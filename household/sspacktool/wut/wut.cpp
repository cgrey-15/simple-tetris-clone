// wut.cpp : Defines the entry point for the application.
//
#define WUT_DIAGNOSTIC_ONLY

//#include <sstream>
#include <vector>
//#include <fstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <charconv>
#include <regex>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
#include <CLI/CLI.hpp>
#include "wut.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define CUTE_PNG_IMPLEMENTATION
#include "cute_headers/cute_png.h"

template<typename It>
struct iterator_tag {
	It       start;
	uint8_t length;
};


enum class CmdArgsStatus : int32_t{
	success = 0,
	no_args,
	path_not_found,
	mismatching_filenames,
	no_confirmation,
	no_pure_sequential_naming
};

void applyEscapeCharsRegex(std::string& str) {
	using namespace std::string_literals;
	size_t currPosition{};
	size_t foundPos;
	
	const auto e = "\\."s;

	while ((foundPos = str.find('.', currPosition)) != std::string::npos) {
		currPosition = foundPos;
		str.replace(foundPos, 1, e);
		currPosition += 2;
	}

	return;
}

const char* failedOperationCause(CmdArgsStatus status) {
	if (status == CmdArgsStatus::no_args) {
		return "USAGE: wut [-y] <path-to-bmp-collection>\n";
	}
	else if (status == CmdArgsStatus::path_not_found) {
		return "Oh. That path does not exist most likely. Exit.\n";
	}
	else if (status == CmdArgsStatus::mismatching_filenames) {
		return "For now, wut only file names with identical alphanumeric characters of bitmap format, appended\nwith numbers. Exit.\n";
	}
	else if( status == CmdArgsStatus::no_confirmation) {
		return "To proceed with the operation, re-run the command and arguments, and add '-y'\n";
	}
	else if (status == CmdArgsStatus::no_pure_sequential_naming) {
		return "wut requires files that are named in sequential order. Exit.\n";
	}
	else {
		return nullptr;
	}
}

using idx_str_map = std::map<int, std::string>;

CmdArgsStatus extractValidFilnamesFromDir(const std::string& filename, std::vector<idx_str_map>& output, std::string& warnings) {
	using namespace std;
	vector<idx_str_map> result{};
	map<string_view, idx_str_map&> tableLookupTable{};

	filesystem::path candidate{filename};
	if (!filesystem::exists(candidate))
		return CmdArgsStatus::path_not_found;

	auto contentIt = filesystem::directory_iterator(filename);
	auto iterEnd = filesystem::end(contentIt);

	vector<string> spriteIsolateList{};
	map<string, regex> spriteSequenceList{};
#ifdef WUT_DIAGNOSTIC_ONLY
	while (contentIt != iterEnd) {
		auto fullFileName = contentIt->path();

		std::string parent;
		{ // delete this maybe, only match regex of filename only and not their location path
			using namespace std::string_literals;
			auto temp = fullFileName.parent_path().string();
			applyEscapeCharsRegex(temp); // only scans for dots
#if 0
			parent = "(" + temp;
#else
			parent = temp;
#endif
		}

		auto filenameStem = fullFileName.stem().string();

		string::iterator ity = end(filenameStem);

		const string digitRegex{ "\\d" };
		constexpr int lastReg = sizeof(digitRegex) - 2;

		int digitCount{};
		for (auto it = (end(filenameStem) - 1); std::isdigit(*it); --it) {
			ity = it;
			++digitCount;
		}
#if 0
		if (ity != end(fileStr))
			fileRegex = parent + "[\\]" + string{ begin(fileStr), ity } + strRepeatedCopy(digitRegex.cbegin(), digitRegex.cend(), digitCount) + "\\." + fullFileName.extension().string().substr(1);
#else
		if (ity != end(filenameStem)) { // this file is a member of an image sequence

			string filenameIdentifier{ begin(filenameStem), ity };

			if (find(spriteIsolateList.cbegin(), spriteIsolateList.cend(), filenameIdentifier) != spriteIsolateList.cend()) {
				warnings += "File '" + contentIt->path().filename().string() + "'There's already a standalone file\n using this name. "
					+ "Wut will not accept the same name used by non-numbered filenames\nand numbered filnames.\n";
				return CmdArgsStatus::mismatching_filenames;
			}

			if (spriteSequenceList.find(filenameIdentifier) == end(spriteSequenceList)) {
				//fileRegex = parent + "\\\\" + filenameIdentifier + "(" + digitRegex + "{" + std::to_string(digitCount) + "})\\." + fullFileName.extension().string().substr(1);
				string fileRegex = filenameIdentifier + "(" + digitRegex + "{" + std::to_string(digitCount) + "})\\." + fullFileName.extension().string().substr(1);
				spriteSequenceList[filenameIdentifier] = regex{ fileRegex };
				result.emplace_back();
				tableLookupTable.insert({ string_view(spriteSequenceList.find(filenameIdentifier)->first), result.back() });
			}

			auto& theRegex = spriteSequenceList[filenameIdentifier];
			smatch nameFormattingMatch{};
			string filename{ contentIt->path().filename().string() };
			bool success = regex_match(cbegin(filename), cend(filename), nameFormattingMatch, theRegex);
			int outputIndexKey = -1;
			if (success) {
				outputIndexKey = std::stoi(nameFormattingMatch[1]);
				//std::cout << "SUCCESS for filename -> '" << filenameStem << "' :)\n";
			}
			else {
				warnings += "File '" + filenameStem + "' is similar to a filename already scanned, but doesn't follow same formatting\n"
					+ "(an image sequence must have the same number of digits).\n";
				return CmdArgsStatus::mismatching_filenames;
			}
			tableLookupTable.find(filenameIdentifier)->second.insert({ outputIndexKey, contentIt->path().string() });

		}
		else { // this is an independent image
			spriteIsolateList.push_back(filenameStem);
			if (spriteSequenceList.find(filenameStem) != spriteSequenceList.cend()) {
				warnings += "File '" + contentIt->path().filename().string() + "'There's already a sequence of file(s)\n that start with this name. "
					+ "Wut will not accept the same name used by non-numbered filenames\nand numbered filnames.\n";
				return CmdArgsStatus::no_pure_sequential_naming;
			}
			result.push_back({ {{-1, contentIt->path().string()}} });
			tableLookupTable.insert({ spriteIsolateList.back(), result.back() });
			//tableLookupTable[filenameStem] = result.back();
		}
#endif

		++contentIt;




	}

#endif

#if 0
	auto fullFileName = contentIt->path();
	std::string parent;
	{
		using namespace std::string_literals;
		auto temp = fullFileName.parent_path().string();
		applyEscapeCharsRegex(temp); // only scans for dots
#if 0
		parent = "(" + temp;
#else
		parent = temp;
#endif
	}
	auto fileStr = fullFileName.stem().string();

	std::string fileRegex{};
	{
		string::iterator ity = end(fileStr);

		const string digitRegex{ "\\d" };
		constexpr int lastReg = sizeof(digitRegex) - 2;

		int digitCount{};
		for (auto it = (end(fileStr)-1); std::isdigit(*it); --it) {
			ity = it;
			++digitCount;
		}
#if 0
		if (ity != end(fileStr))
			fileRegex = parent + "[\\]" + string{ begin(fileStr), ity } + strRepeatedCopy(digitRegex.cbegin(), digitRegex.cend(), digitCount) + "\\." + fullFileName.extension().string().substr(1);
#else
		if (ity != end(fileStr))
			fileRegex = parent + "\\\\" + string{ begin(fileStr), ity } + "(" + digitRegex + "{" + std::to_string(digitCount) + "})\\." + fullFileName.extension().string().substr(1);
#endif

	}

	regex theRegex{ fileRegex, std::regex_constants::syntax_option_type::ECMAScript };

	std::cout << "REGEX EXPRESSION: " << fileRegex << "\n";

	std::map<int, filesystem::directory_entry> files{};

	for (auto& dirEntry : contentIt) {
		auto fileStr = dirEntry.path().string();
		smatch okay{};
		bool success = regex_match(cbegin(fileStr), cend(fileStr), okay, theRegex);
		int key = -1;
		if (success) {
			key = std::stoi(okay[1]);
			std::cout << "SUCCESS for filename -> '" << fileStr << "' :)\n";
		}
		else {
			//return CmdArgsStatus::mismatching_filenames;
			cerr << "Mismatch after scanning filename '" << fileStr << "'\n";
		}
		std::cout << fileStr << "\nOUT: " << success << ": ";
		for (auto& match : okay) {
			std::cout << match << " |";
		}
		std::cout << "\n\n";
		//output.push_back(dirEntry.path().string());
		files.insert({ key, dirEntry });
	}
	int prevValue = std::begin(files)->first - 1;

	std::map<int, std::string> resultant{};
	for (auto& [key, value] : files) {
		if ((key - 1) != prevValue) {
			return CmdArgsStatus::no_pure_sequential_naming;
		}
		resultant.insert({ key, value.path().string() });

		prevValue = key;
	}
	output = resultant;
#endif

	output = result;
	return CmdArgsStatus::success;
}

void write_filey(void* context, void* data, int size) {
	std::vector<cp_image_t>* images = reinterpret_cast<std::vector<cp_image_t>*>(context);

	cp_image_t img_out = cp_load_png_mem(data, size);
	images->push_back(img_out);
}

struct FileInfo {
	std::string filename;
	int width;
	int height;
	int channels;
	int seqIndex;
};

int main(const int argc, const char* argv[])
{
	stbi_write_tga_with_rle = 0;
	std::cout << "Hello CMake.\n";
	int x;
	int y;
	int channel;
	//std::vector<std::vector<unsigned char>> muffins{};

	std::string inputPath{};
	std::string outputPath{};
	std::string outputName{};

	CLI::App app{"This is wut app."};
	app.allow_extras(true);

	auto confimationFlag = app.add_flag("-y", "Confirmation option. Don't proceede without it.");
	confimationFlag->required();
	confimationFlag->allow_extra_args(false);

	auto outputOpt = app.add_option("-d", outputPath, "Path to output atlas to.");

	auto outNameOpt = app.add_option("-o", outputName, "Name you want to give to the processed files");


	
	CLI11_PARSE(app, argc, argv);

	auto hmmnow = confimationFlag->results()[0];
	//if (!hmmnow.empty())
	//	std::cout << "What??: " << hmmnow << " Count: " << hmmnow.size() << "\n";

	auto extraArgs = app.remaining();
	if (!outputPath.empty() ) {
		if (outputPath.back() != '/' && outputPath.back() != '\\') {
			outputPath.push_back('/');
		}
	}
	else {
		outputPath = "./";
	}

	std::string dirToProcess{};

	if (!extraArgs.empty()) {
		auto size = extraArgs.size();
		dirToProcess = extraArgs[0];
	}
	else {
		std::cerr << "Something's wrong with CLI...\n";
	}

	//std::cout << "Extra args: '" << dirToProcess << "'\n";

	std::map<int, std::string> frameIndiciesMap{};
	std::vector<idx_str_map> frameIndicesMap2{};
	std::string warnings{};
	auto status = extractValidFilnamesFromDir(dirToProcess, frameIndicesMap2, warnings);

	if (!warnings.empty())
		std::cout << warnings;

	if (status != CmdArgsStatus::success) {
		std::cout << failedOperationCause(status);
		return -1;
	}
	/*
	else {
		for (auto& map : frameIndicesMap2) {
			for (auto& [key, name] : map) {
				std::cout << name << "\n";
			}
		}
	}*/



	std::vector<FileInfo> imageMetadata{};
	for (auto& map_e : frameIndicesMap2) {
		for (auto& [key, name] : map_e) {
			imageMetadata.emplace_back();
			imageMetadata.back().filename = name;
			imageMetadata.back().seqIndex = key;
			//imageMetadata.push_back({ name });
		}
	}

	std::vector<cp_image_t> convertedPngData{};

#ifdef WUT_DIAGNOSTIC_ONLY
	for (auto& [fname, x, y, channels, unused] : imageMetadata) {
		unsigned char* raw_data = stbi_load(fname.c_str(), &x, &y, &channels, 0);
		if (!raw_data) {
			std::exit(42);
		}


		int what = stbi_write_png_to_func(write_filey, &convertedPngData, x, y, channels, raw_data, 0);
		if (!what) {
			std::cerr << "uh-oh: " << what << "\n";
		}

		stbi_image_free(raw_data);
	}


	std::unique_ptr<cp_atlas_image_t[]> images{ new cp_atlas_image_t[imageMetadata.size()] };

	int totalPixelArea{};
	int squareLen = 2 * 2;
	for (auto& image : convertedPngData) {
		totalPixelArea += (image.h * image.w);
	}
	while (((squareLen *= 2) * squareLen) < totalPixelArea);



	auto atlas_blob = cp_make_atlas(squareLen, squareLen, convertedPngData.data(), convertedPngData.size(), images.get());
	//auto sim = atlas_blob.pix;

#if 0
	for (int i = 0; i < imageMetadata.size(); ++i) {
		int ind = images[i].img_index;
		std::pair<float, float> minni = { images[i].minx, images[i].miny };
		std::pair<float, float> maxxi = { images[i].maxx, images[i].maxy };
		std::cout << "img_index: " << ind << "\n  min: {" << minni.first << ", " << minni.second << "}\n  max: {" << maxxi.first << ", " << maxxi.second << "}\n";
	}
#endif

#endif
	std::vector<const char*> filenamey{};

	// Extract filename from path for cute_header's atlas png export
	std::for_each(imageMetadata.begin(), imageMetadata.end(), [&filenamey](FileInfo& a) {
		auto found = a.filename.rfind('\\');
		if (found == std::string::npos)
			found = a.filename.rfind('/');
		if (found != std::string::npos) {
			a.filename = a.filename.substr(found + 1);
			filenamey.push_back(a.filename.data());
		}
		});

	// Extract filename and indicies determined by cp_make_atlas() function for json output
	std::vector < std::pair<const int, std::string>> output_filenames{};
#ifdef WUT_DIAGNOSTIC_ONLY
	output_filenames.reserve(filenamey.size());//(frameIndiciesMap.size());
	for (int i = 0, len = imageMetadata.size(); i < len; ++i) {
		std::string_view strv{ imageMetadata[images[i].img_index].filename };
		auto found = strv.rfind('\\');//frameIndiciesMap[images[i].img_index].rfind('\\');
		if (found == std::string_view::npos) {
			found = strv.rfind('/');//frameIndiciesMap[images[i].img_index].rfind('/');
		}
		if (found != std::string::npos) {
			output_filenames.emplace_back(imageMetadata[images[i].img_index].seqIndex, /*frameIndiciesMap[images[i].img_index]*/strv.substr(found + 1));
		}
		else {
			output_filenames.emplace_back(imageMetadata[images[i].img_index].seqIndex, strv);
		}
	}
#endif

	nlohmann::json jsonObject = nlohmann::json::array();
	{
		int jArrCurrI = 0;
		for (int i = 0; i < output_filenames.size();) {
#if 0
			jsonObject.push_back({
				{"label", }
				});
#endif
			jsonObject.emplace_back();
#if 0
			jsonObject[i]["filename"] = output_filenames[i].second;
			jsonObject[i]["index"] = output_filenames[i].first;
			jsonObject[i]["dimension"] = { {"width", images[i].w}, {"height", images[i].h} };
			jsonObject[i]["minimum-uv"] = { {"u", images[i].minx}, {"v", images[i].miny} };
			jsonObject[i]["maximum-uv"] = { {"u", images[i].maxx}, {"v", images[i].maxy} };
#endif

#if 1
						std::string::const_iterator it;
						for (it = std::begin(output_filenames[i].second) + output_filenames[i].second.rfind('.'); std::isdigit(*(it - 1)); --it) {}
						jsonObject.back()["label"] = std::string{std::cbegin(output_filenames[i].second), it};
						jsonObject.back()["dimension"] = { {"width", images[i].w}, {"height", images[i].h} };
						jsonObject.back()["frames"] = nlohmann::json::array();

			{
				int prevIndex = -1;

				do {

					//auto* huh = &output_filenames[i];
					//std::cout << output_filenames[i].second << "\n";
					//auto processedImgPtr = std::find_if(images.get(), images.get() + imageMetadata.size(), [i](const decltype(images)::element_type& v) {return v.img_index == i; });
					jsonObject.back()["frames"].emplace_back();
					jsonObject.back()["frames"].back()["minimum-uv"] = { {"u", images[i].minx}, {"v", 1.f - images[i].miny} };
					jsonObject.back()["frames"].back()["maximum-uv"] = { {"u", images[i].maxx}, {"v", 1.f - images[i].maxy} };
					prevIndex = output_filenames[i].first;
					//++i;
				} while ( i < output_filenames.size() && output_filenames[i++].first != -1 && prevIndex < output_filenames[i].first);
			}
#endif
		}
	}


	//std::cout << "OUTPUT_JSON:\n" << jsonObject.dump(3);
	std::ofstream output_io_json{ outputPath + outputName + ".json" };
	output_io_json << jsonObject.dump(3);
	output_io_json.close();

	if (outputName.empty() || outputName == "") {
		outputName = "hello";
	}
#ifdef WUT_DIAGNOSTIC_ONLY
	if (true || !std::filesystem::exists(outputPath+outputName+".tga")) {
		std::cout << "Writing texture-atlas...\n";
		stbi_write_tga((outputPath+outputName+".tga").c_str(), squareLen, squareLen, 4, atlas_blob.pix);
	}

	if (true || !std::filesystem::exists(outputPath+outputName+".png")) {
		int pngWriteResul = cp_default_save_atlas((outputPath+outputName+".png").c_str(), (outputPath+outputName+".txt").c_str(), &atlas_blob, images.get(), imageMetadata.size(), filenamey.data());
	}

	cp_free_png(&atlas_blob);
#endif

#if 0
	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit unit = clang_parseTranslationUnit(index,
		"MyLittleHeader.hpp", nullptr, 0,
		nullptr, 0,
		CXTranslationUnit_None);
	if (unit == nullptr) {
		std::cerr << "Can't parse this document at this time. Exit.\n";
		std::exit(43);
	}

	CXCursor cursor = clang_getTranslationUnitCursor(unit);

	clang_visitChildren(
		cursor,
		[](CXCursor c, CXCursor parent, CXClientData client_data)
		{
			std::cout << "Cursor '" << clang_getCursorSpelling(c) << "' of kind '"
				<< clang_getCursorKindSpelling(clang_getCursorKind(c)) << "'\n";
			return CXChildVisit_Recurse;
		},
		nullptr);

	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
#endif
	//unsigned char* raw_data = stbi_load("Animation_folder0001.bmp", &x, &y, &channel, 0);
	//stbi_image_free(raw_data);
	return 0;
}

#if 0
std::ostream& operator<<(std::ostream& stream, const CXString& str) {
	stream << clang_getCString(str);
	clang_disposeString(str);
	return stream;
}
#endif
