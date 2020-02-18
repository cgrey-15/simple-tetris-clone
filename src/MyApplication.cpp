#include <cassert>

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/ImageView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>


#include <Magnum/GL/Renderer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>


#include <Magnum/Shaders/Flat.h>


#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/RigidMatrixTransformation2D.h>

#include <Magnum/Timeline.h>

#include "Sprite.h"
#include "RenderTiles.h"
#include "TexturedTriangleShader.h"
#include "Utilities.hpp"

#include "TetrisBoard.h"
#include "TetronimoRendering.h"
#include "Tetronimo.h"
#include "TetrominoBlobRender.h"

#include "MacroIOMessage.h"

using namespace Magnum;

namespace custard {


	template <auto N>
	class StateBundle {
		static_assert(std::is_enum<decltype(N)>::value, "Not an enum");
		static constexpr size_t size = static_cast<size_t>(N);

		using enum_type_t = decltype(N);

		std::array<bool, size> data_;

	public:
		StateBundle() {
			for (int i = 0; i < data_.size(); ++i)
				data_[i] = false;
		}

#if 1
		template<enum_type_t I>
		void toggle() {
			std::get<static_cast<size_t>(I)>(data_) = !std::get<static_cast<size_t>(I)>(data_);
		}
#else
		template<int i = j>
		void toggle(const int j) {
			std::get<i>(data_) = !std::get<i>(data_);
		}
#endif

		template<enum_type_t I>
		typename decltype(data_)::value_type is() {
			return std::get<static_cast<size_t>(I)>(data_);
		}
	};

}

tetris_clone::Tetronimo::Flavor toTetro(const char c) {
	tetris_clone::Tetronimo::Flavor result{};
	switch (c) {
		using namespace tetris_clone;
	case 'S': result = Tetronimo::Flavor::S;
		break;
	case 'Z': result = Tetronimo::Flavor::Z;
		break;
	case 'T': result = Tetronimo::Flavor::T;
		break;
	case 'J': result = Tetronimo::Flavor::J;
		break;
	case 'L': result = Tetronimo::Flavor::L;
		break;
	case 'I': result = Tetronimo::Flavor::I;
		break;
	case 'O': result = Tetronimo::Flavor::O;
		break;
	default: result = Tetronimo::Flavor::I;
	}
	return result;
}
tetris_clone::Tetronimo::Rotation toTetroRot(const char c) {
	tetris_clone::Tetronimo::Rotation rot{};

	switch (c) {
		using namespace tetris_clone;
	case 'n': rot = Tetronimo::Rotation::North;
		break;
	case 'e': rot = Tetronimo::Rotation::East;
		break;
	case 's': rot = Tetronimo::Rotation::South;
		break;
	case 'w': rot = Tetronimo::Rotation::West;
		break;
	default: rot = rot;
	}
	return rot;
}
char fromTetroRotToChar(const tetris_clone::Tetronimo::Rotation r) {
	char res;
	switch (r) {
		using namespace tetris_clone;
	case Tetronimo::Rotation::North: res = 'n';
		break;
	case Tetronimo::Rotation::East: res = 'e';
		break;
	case Tetronimo::Rotation::South: res = 's';
		break;
	case Tetronimo::Rotation::West: res = 'w';
		break;
	default: res = '\0';
	}
	return res;
}
char fromTetroToChar(const tetris_clone::Tetronimo::Flavor f) {
	char result{};
	switch(f) {
		using namespace tetris_clone;
	case Tetronimo::Flavor::S: result = 'S';
		break;
	case Tetronimo::Flavor::Z: result = 'Z';
		break;
	case Tetronimo::Flavor::T: result = 'T';
		break;
	case Tetronimo::Flavor::J: result = 'J';
		break;
	case Tetronimo::Flavor::L: result = 'L';
		break;
	case Tetronimo::Flavor::I: result = 'I';
		break;
	case Tetronimo::Flavor::O: result = 'O';
		break;
	default: result = '\0';
	}
	return result;
}

struct TetrominoObject2d : public RudimentarySprite::Object2D {
	explicit TetrominoObject2d(RudimentarySprite::Object2D* holder = nullptr) : RudimentarySprite::Object2D{ holder }{}
	int x;
	int y;
};
class MyApplication: public Platform::Application {
    public:
		typedef	Magnum::SceneGraph::Scene<SceneGraph::MatrixTransformation2D> Scene2D;
        explicit MyApplication(const Arguments& arguments);

    private:
        void drawEvent() override;
#if 0
		void mousePressEvent(MouseEvent& event) override;
		void mouseReleaseEvent(MouseEvent& event) override;
		void mouseMoveEvent(MouseMoveEvent& event) override;
#endif
		void keyPressEvent(KeyEvent& event) override;
		//void keyReleasedEvent(KeyEvent& event) override;

		void placeTetromino(bool readMode = false);
		void transferTetrominos(int size, int startRow);
		void transferTetrosBack(int rowsCleared);
		void resetRenderer(bool makeVisible);

		int subdivideTetros(RudimentarySprite::Object2D& src, int n, RudimentarySprite::Object2D& dst, int clearedRowSize, int startingRow);
		bool adjacentTiles(tetris_clone::TetronimoRendering::TetrominoObject2D* first, tetris_clone::TetronimoRendering::TetrominoObject2D* second);
		int divisionOperationLatter(RudimentarySprite::Object2D* begin, RudimentarySprite::Object2D* startFrom);
		int divisionOperationFormer(RudimentarySprite::Object2D* begin, RudimentarySprite::Object2D* startFrom);

		bool tetrosAboveRow(const RudimentarySprite::Object2D* firstChild, int clearedRowSize, int startingRow) const;
		
		typedef std::pair<int, int>(*pointer_things)[4];
		pointer_things packCoordsTogether(RudimentarySprite::Object2D* ptr) const;
		void debugPrint01_(const std::tuple<char, char, int, int>);

		struct TriangleVertex {
			Vector2 position;
			Vector2 textureCoordinates;
		};

		enum class RegState : int8_t {
			agitated = 0,
			rotated,
			dropping,
			placement_mode,
			move_blob_mode_start,
			move_blob_mode,
			beginning_drop,
			COUNT
		};
		enum class OtherState : int8_t {
			choose_mode = 0,
			macro_write_mode,
			macro_read_mode,
			beginning_readstep,
			beginning_macro_read_mode,
			macro_read_mode_paused,
			COUNT
		};

		const char _macroFilename[13];

		Debug _out;

		Shaders::Flat2D _shader;
		GL::Texture2D _texture;

		Timeline _tl;

		tetris_clone::Tetronimo _te;
		tetris_clone::Tetronimo _oldTe{ tetris_clone::Tetronimo::Flavor::T };
		tetris_clone::TetrisBoard _tb;

		Scene2D                         _scene;
		RudimentarySprite::Object2D _camObject;
		SceneGraph::Camera2D*          _camera;

		//Containers::Array<Containers::Optional<RudimentarySprite>> _activeBlocks;
		//Containers::Optional<RudimentarySprite> _spritey;
		Containers::Optional<RudimentaryRenderTiles> _bg;

		SceneGraph::DrawableGroup2D _drawables;
		const Matrix3 _lol;
		RudimentarySprite::Object2D* _tetronimoSprite;
		tetris_clone::TetronimoRendering* _renderer;

		custard::StateBundle<RegState::COUNT> _testMachine;
		custard::StateBundle<OtherState::COUNT> _testIoMachine;

		MacroIOMessage _fileStream;

		int _i;
		int _nRowsCleared;

		Magnum::Vector2 _originalPos;

		Magnum::Float _timeSinceDrop;
		Magnum::Float _prevTime;

		Magnum::Float _prevTimeReadie;

		RudimentarySprite* _tetroBlocks01;

		RudimentarySprite::Object2D* _bottomBlob;
		RudimentarySprite::Object2D* _droppableBlob;

		RudimentarySprite::Object2D* _renderTilePool;

		tetris_clone::TetrisBoard::RowsClearedT _lingeringClearedRows;
};

constexpr Float canvasLengthTexCoor = 16.f / 64.f;
constexpr Float secondRowCol = canvasLengthTexCoor;
constexpr Float thirdCol = canvasLengthTexCoor * 2.f;
constexpr Float fourthCol = canvasLengthTexCoor * 3.f;

		
const Containers::Array<Magnum::Vector2> globalTexCoords = Containers::Array<Vector2>(Containers::InPlaceInit, { {0.f, 0.f},
	{secondRowCol, 0.f},
	{0.f, secondRowCol},
	{secondRowCol, secondRowCol},
	{thirdCol, 0.f},
	{thirdCol, secondRowCol},
	{fourthCol, 0.f},
	{fourthCol, secondRowCol} });


MyApplication::MyApplication(const Arguments& arguments) : Platform::Application{ arguments, Configuration{}.setTitle("TetrisClonei") },
_i{},
_lol{},
_tl{},
_te{ tetris_clone::Tetronimo::Flavor::T },
_tb{},
_scene{},
_bg{},
_shader{ Shaders::Flat2D::Flag::Textured },
_tetronimoSprite{},
_renderer{},
_testMachine{},
_testIoMachine{},
_fileStream{},
_nRowsCleared{},
_prevTime{},
_prevTimeReadie{},
_timeSinceDrop{},
_tetroBlocks01{},
_bottomBlob{},
_droppableBlob{},
_renderTilePool{},
_macroFilename{"macro_file"},
_lingeringClearedRows{}
{
    /* TODO: Add your initialization code here */

#if 1

	using namespace Math::Literals;
	
#endif

	_camObject
		.setParent(&_scene)
		.translate(Magnum::Vector2{})
		.scale(Vector2{ 600.f, 600.f })
		.translate({200.f, 150.f});

	(*(_camera = new SceneGraph::Camera2D{ _camObject }))
		.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
		.setProjectionMatrix(Matrix3::projection(Vector2{ 4.0f/3.0f, 1.f }))
		.setViewport(GL::defaultFramebuffer.viewport().size());


	PluginManager::Manager<Trade::AbstractImporter> manager;
	Containers::Pointer<Trade::AbstractImporter> importer =
		manager.loadAndInstantiate("TgaImporter");
	if (!importer) 
		std::exit(1);

	const Utility::Resource rs{ "textured-triangle-data" };
	if (!importer->openData(rs.getRaw("sprites.tga")))
		std::exit(2);

	Containers::Optional<Trade::ImageData2D> image = importer->image2D(0);
	CORRADE_INTERNAL_ASSERT(image);
	_texture.setWrapping(GL::SamplerWrapping::ClampToEdge)
		.setMagnificationFilter(GL::SamplerFilter::Nearest)
		.setMinificationFilter(GL::SamplerFilter::Nearest)
		.setStorage(1, GL::TextureFormat::RGB8, { 64, 64 })//image->size())
		.setSubImage(0, {0, 0}, *image);
	GL::Renderer::setClearColor(0xa5c9ea_rgbf);

	auto* obj1 = new RudimentaryRenderTiles::Object2D{ &_scene };
	obj1->setTransformation(_lol);

	Vector2 bgTextureCoords[4] =
	{ supp::supporianCoordsUnconvert<64,64>(0, 48),
	supp::supporianCoordsUnconvert<64,64>(16, 48),
	supp::supporianCoordsUnconvert<64,64>(16, 32),
	supp::supporianCoordsUnconvert<64,64>(0, 32) };
	_bg.emplace(16, 16, 20, 10, _texture, *obj1, _drawables, _shader, bgTextureCoords);

	obj1->translate({5.f*16, 10.f*16});

	//_spritey.emplace( *obj, 16, 16, (1.f / 8.f), _texture, _tl, _drawables, _shader );

	auto* me = new RudimentarySprite::Object2D{ &_scene };
	_tetronimoSprite = me;
	me->translate({ static_cast<Float>((_te.x()) * 16), static_cast<Float>((_te.y()) * 16) });
	auto* you = new tetris_clone::TetronimoRendering{ *me, 16, 16, _shader, _texture, _tl, _drawables };
	you->redoTetronimoRender(_te, globalTexCoords.data(), 8, true);
	_renderer = you;

	_scene.addChild<RudimentarySprite::Object2D>();
	_bottomBlob = _scene.children().last();
	_scene.addChild<RudimentarySprite::Object2D>();
	_droppableBlob = _scene.children().last();
	_scene.addChild<RudimentarySprite::Object2D>();
	_renderTilePool = _scene.children().last();
	//_spritey->setTexCoords(std::move(texCoords));

	_testMachine.toggle<RegState::placement_mode>();

#if 0
	Debug{} << "Hello! This application is running on"
		<< GL::Context::current().version() << "using"
		<< GL::Context::current().rendererString();
#endif
	_tl.start();
	//_spritey->getPrevTime();
}

void MyApplication::drawEvent() {
	GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);//|GL::FramebufferClear::Depth);
	using namespace Math::Literals;

#if 1
	if (_testMachine.is<RegState::move_blob_mode_start>()) {
		if (!_testMachine.is<RegState::move_blob_mode>())
			_testMachine.toggle<RegState::move_blob_mode>();

		assert(!(_testMachine.is<RegState::beginning_drop>() or _testMachine.is<RegState::dropping>()), "Drop mode and move mode on at same time!");
		assert(!_testMachine.is<RegState::placement_mode>(), "Placement-mode and move-blob mode are on at the same time!");

		_testMachine.toggle<RegState::move_blob_mode_start>();
	}
#endif 

	if (_testMachine.is<RegState::agitated>()) {
		auto delta = Vector2{16.f*(_te.x()), 16.f*(_te.y())} - _tetronimoSprite->transformationMatrix().translation();
		_tetronimoSprite->translate(delta);
		_testMachine.toggle<RegState::agitated>();
	}
	else if (_testMachine.is<RegState::rotated>()) {
		_renderer->updateTetronimoRender(_te, true);
		_testMachine.toggle<RegState::rotated>();
	}

	if (_testMachine.is<RegState::dropping>()) {
		if (_testMachine.is<RegState::beginning_drop>()) {
			_prevTime = _tl.previousFrameTime() + _tl.previousFrameDuration() + (1.f);// / 8.f);
			_testMachine.toggle<RegState::beginning_drop>();
			assert(!_testMachine.is<RegState::beginning_drop>(), "Drop-started state is already false");
			_droppableBlob->translate({0.f, -4.f});
		}
		if (_tl.previousFrameTime() > _prevTime) {
			auto rowsJustCleared = _nRowsCleared;
			_droppableBlob->translate({ 0.f, -((16.f*rowsJustCleared)-4.f) });
			_prevTime = {};
			if (_lingeringClearedRows.sizeFirst == 0) {
				assert(_testMachine.is<RegState::dropping>(), "Dropping state is already false!");
				_testMachine.toggle<RegState::dropping>();
				assert(!_testMachine.is<RegState::placement_mode>(), "Placement mode was already set on!");
				_testMachine.toggle<RegState::placement_mode>();
			}
			else {
				_testMachine.toggle<RegState::beginning_drop>();
				_nRowsCleared = _lingeringClearedRows.sizeFirst;
			}
			transferTetrosBack(rowsJustCleared);
			_droppableBlob->translate({ 0.f, 16.f * rowsJustCleared});
			if (_lingeringClearedRows.sizeFirst != 0) {
				transferTetrominos(_lingeringClearedRows.sizeFirst, _lingeringClearedRows.rowIndexFirst - rowsJustCleared);
				_lingeringClearedRows.sizeFirst = 0;
			}
			else {
				_te = tetris_clone::Tetronimo::simpleSpawn();
				resetRenderer(true);
			}
		}

	}

	if (_testIoMachine.is<OtherState::macro_read_mode>() and !_testIoMachine.is<OtherState::macro_read_mode_paused>()) { // Initiator HAS to set both 'macro_read_mode' and 'beginning_readstep' flags to TRUE to work properly!
		bool yes{};
		if (_testIoMachine.is<OtherState::beginning_readstep>()) {
			std::tuple<char, char, int, int> result;
			if (_fileStream.hasMore()) {
				yes = true;
				result = _fileStream.read();
				//debugPrint01_(result);
				if (std::get<0>(result) != 'A') {
					auto flavor = toTetro(std::get<0>(result));
					auto rot = toTetroRot(std::get<1>(result));
					_te = tetris_clone::Tetronimo(flavor, { std::get<2>(result), std::get<3>(result) });
					_te.setRotation(rot);
					if (true) {//_testIoMachine.is<OtherState::beginning_macro_read_mode>()) {
						_renderer->updateColor(flavor);
						_renderer->updateTetronimoRender(_te);
						//resetRenderer(true);
						//_testIoMachine.toggle<OtherState::beginning_macro_read_mode>();
					}
					if (rot != tetris_clone::Tetronimo::Rotation::North)
						_testMachine.toggle<RegState::rotated>();
					//_renderer->updateTetronimoRender(_te)
					_testMachine.toggle<RegState::agitated>();

					_prevTimeReadie = _tl.previousFrameTime() + _tl.previousFrameDuration() + 0.125f;
				}
				else {
					_testIoMachine.toggle<OtherState::macro_read_mode_paused>();
					result = _fileStream.read();
					auto flavor = toTetro(std::get<0>(result));
					auto rot = toTetroRot(std::get<1>(result));
					_te = tetris_clone::Tetronimo(flavor, { std::get<2>(result), std::get<3>(result) });
					_te.setRotation(rot);
					//resetRenderer(true);
					_renderer->updateColor(flavor);
					_renderer->updateTetronimoRender(_te);
					_testMachine.toggle<RegState::agitated>();

					_prevTimeReadie = _tl.previousFrameTime() + _tl.previousFrameDuration() + 0.125f;
				}
			}
			else {
				_fileStream.close();
				_te = tetris_clone::Tetronimo::simpleSpawn();
				_renderer->updateColor(_te);
				_renderer->updateTetronimoRender(_te);
				_testMachine.toggle<RegState::agitated>();
				//resetRenderer(true);
				_testIoMachine.toggle<OtherState::macro_read_mode>(); // final exit from 'macro_read_mode'
			}
			_testIoMachine.toggle<OtherState::beginning_readstep>();
		}
		else if (_tl.previousFrameTime() > _prevTimeReadie) {
			_prevTimeReadie = {};
			_testIoMachine.toggle<OtherState::beginning_readstep>(); //set to true!
			placeTetromino(true);
			resetRenderer(true);
		}
	}

	_camera->draw(_drawables);

	_tl.nextFrame();

    swapBuffers();
	redraw();
}

void MyApplication::keyPressEvent(KeyEvent& event) {
	auto keyPress = event.key();

#define _TEST_DROPPABLE
	if (keyPress == KeyEvent::Key::Up) {
		if (_testMachine.is<RegState::placement_mode>() ){
			_te.moveNBlocksForwardY(1);
			//_teAgitated = true;
			_testMachine.toggle<RegState::agitated>();
		}
		else if (_testMachine.is<RegState::move_blob_mode>() ){
#ifdef _TEST_DROPPABLE
			_droppableBlob->translate({ 0.f, 16.f });
#else
			//_bottomBlob->translate({ 0.f, 16.f });
			_renderTilePool->translate({ 0.f, 16.f });
#endif
		}
	}
	if (keyPress == KeyEvent::Key::Down) {
		if (_testMachine.is<RegState::placement_mode>() ){
			if (_te.canRegressY(_tb)) {
				_te.moveNBlocksForwardY(-1);
				//_teAgitated = true;
				_testMachine.toggle<RegState::agitated>();
			}
		}
		else if (_testMachine.is<RegState::move_blob_mode>() ){
#ifdef _TEST_DROPPABLE
			_droppableBlob->translate({ 0.f, -16.f });
#else
			//_bottomBlob->translate({ 0.f, -16.f });
			_renderTilePool->translate({ 0.f, -16.f });
#endif
		}
	}
	if (keyPress == KeyEvent::Key::Left) {
		if (_testMachine.is<RegState::placement_mode>() ){
			if (_te.canRegressX(_tb)) {
				_te.moveNBlocksForwardX(-1);
				//_teAgitated = true;
				_testMachine.toggle<RegState::agitated>();
			}
		}
		else if (_testMachine.is<RegState::move_blob_mode>() ){
#ifdef _TEST_DROPPABLE
			_droppableBlob->translate({ -16.f, 0.f });
#else
			//_bottomBlob->translate({ -16.f, 0.f });
			_renderTilePool->translate({ -16.f, 0.f });
#endif
		}
	}
	if (keyPress == KeyEvent::Key::Right) {
		if (_testMachine.is<RegState::placement_mode>() ){
			if (_te.canAdvanceX(_tb)) {
				_te.moveNBlocksForwardX(1);
				_testMachine.toggle<RegState::agitated>();
			}
		}
		else if (_testMachine.is<RegState::move_blob_mode>() ){
#ifdef _TEST_DROPPABLE
			_droppableBlob->translate({ 16.f, 0.f });
#else
			//_bottomBlob->translate({ 16.f, 0.f });
			_renderTilePool->translate({ 16.f, 0.f });
#endif
		}
	}
#undef _TEST_DROPPABLE
	
	if (keyPress == KeyEvent::Key::R) {
		--_te;
		_testMachine.toggle<RegState::rotated>();
	}
	if (keyPress == KeyEvent::Key::E) {
		++_te;
		_testMachine.toggle<RegState::rotated>();
	}

	if (keyPress == KeyEvent::Key::G and /*_moveBlobMode and*/ _testMachine.is<RegState::move_blob_mode>()) {
		_droppableBlob->translate(-(_droppableBlob->transformation().translation() - _originalPos));
		_originalPos = {};

		assert(_testMachine.is<RegState::move_blob_mode>(), "Already false");
		_testMachine.toggle<RegState::move_blob_mode>();

		//assert(!_testMachine.is<RegState::placement_mode>(), "Already true!");
		//_testMachine.toggle<RegState::placement_mode>();

		assert(!(_testMachine.is<RegState::beginning_drop>() or _testMachine.is<RegState::dropping>()), "Already dropping!?");
		_testMachine.toggle<RegState::beginning_drop>();
		_testMachine.toggle<RegState::dropping>();

		//_te = tetris_clone::Tetronimo::simpleSpawn();
		//resetRenderer(true);
	}
	else if (keyPress == KeyEvent::Key::G)
		int i = 2;

	if (keyPress == KeyEvent::Key::Y and _testMachine.is<RegState::placement_mode>() ) {

		if (_testIoMachine.is<OtherState::macro_write_mode>()) {
			_fileStream.write(fromTetroToChar(_te.flavor()), fromTetroRotToChar(_te.rotation()), _te.x(), _te.y());
		}

		placeTetromino();
		_te = tetris_clone::Tetronimo::simpleSpawn();
		resetRenderer(true);
	}

	if (keyPress == KeyEvent::Key::Q and _testMachine.is<RegState::placement_mode>() ) {

		if (_testIoMachine.is<OtherState::macro_write_mode>()) {
			_fileStream.write(fromTetroToChar(_te.flavor()), fromTetroRotToChar(_te.rotation()), _te.x(), _te.y());
		}
		placeTetromino();
		//_te = tetris_clone::Tetronimo::simpleSpawn();
		//resetRenderer(true);

		auto results = _tb.clearFullRows();
		_nRowsCleared = results.sizeFirst;

		if (results.sizeSecond != 0) {
			_lingeringClearedRows.sizeFirst = results.sizeSecond;
			_lingeringClearedRows.rowIndexFirst = results.rowIndexSecond;
		}

		transferTetrominos(results.sizeFirst, results.rowIndexFirst);

		_originalPos = _droppableBlob->transformation().translation();
		//_moveBlobMode = true;
	}

	if (keyPress == KeyEvent::Key::V and !_testIoMachine.is<OtherState::macro_read_mode>()) {
		if (!_testIoMachine.is<OtherState::macro_write_mode>()) {
			_fileStream.startStream(true, _macroFilename);
			_testIoMachine.toggle<OtherState::macro_write_mode>();
		}
		else {
			_fileStream.close();
			_testIoMachine.toggle<OtherState::macro_write_mode>();
		}
	}
	if (keyPress == KeyEvent::Key::B and !_testIoMachine.is<OtherState::macro_write_mode>()) {
		if (!_testIoMachine.is<OtherState::macro_read_mode>()) {
			_fileStream.startStream(false, _macroFilename);
			//_renderer->purgeTiles(*_renderTilePool);
			_testIoMachine.toggle<OtherState::macro_read_mode>();
			_testIoMachine.toggle < OtherState::beginning_readstep>();
			_testIoMachine.toggle<OtherState::beginning_macro_read_mode>();
		}
	}

	if (keyPress == KeyEvent::Key::One ){//and _testMachine.is<RegState::do_spawn>() and !_testMachine.is<RegState::random_spawn_mode>()) {
		_te.replaceFlavorWith(tetris_clone::Tetronimo::Flavor::S);
		_renderer->updateTetronimoRender(_te);
		_renderer->updateColor(_te);
	}
	if (keyPress == KeyEvent::Key::Two) {
		_te.replaceFlavorWith(tetris_clone::Tetronimo::Flavor::Z);
		_renderer->updateTetronimoRender(_te);
		_renderer->updateColor(_te);
	}
	if (keyPress == KeyEvent::Key::Three) {
		_te.replaceFlavorWith(tetris_clone::Tetronimo::Flavor::T);
		_renderer->updateTetronimoRender(_te);
		_renderer->updateColor(_te);
	}
	if (keyPress == KeyEvent::Key::Four) {
		_te.replaceFlavorWith(tetris_clone::Tetronimo::Flavor::J);
		_renderer->updateTetronimoRender(_te);
		_renderer->updateColor(_te);
	}
	if (keyPress == KeyEvent::Key::Five) {
		_te.replaceFlavorWith(tetris_clone::Tetronimo::Flavor::L);
		_renderer->updateTetronimoRender(_te);
		_renderer->updateColor(_te);
	}
	if (keyPress == KeyEvent::Key::Six) {
		_te.replaceFlavorWith(tetris_clone::Tetronimo::Flavor::I);
		_renderer->updateTetronimoRender(_te);
		_renderer->updateColor(_te);
	}
	if (keyPress == KeyEvent::Key::Seven) {
		_te.replaceFlavorWith(tetris_clone::Tetronimo::Flavor::O);
		_renderer->updateTetronimoRender(_te);
		_renderer->updateColor(_te);
	}
	if (keyPress == KeyEvent::Key::N and _testIoMachine.is<OtherState::macro_read_mode>()) {
		if (_testIoMachine.is<OtherState::macro_read_mode_paused>()) {
			//placeTetromino();
			_testIoMachine.toggle<OtherState::beginning_readstep>();
			_testIoMachine.toggle<OtherState::macro_read_mode_paused>();
		}
	}
	if (keyPress == KeyEvent::Key::C and _testIoMachine.is<OtherState::macro_write_mode>()) {
		_fileStream.write('A', 'A', 0, 0);
	}

}

// TODO: This function is bugged with object slicing!
void MyApplication::placeTetromino(bool readMode) {

	_bottomBlob->addChild<RudimentarySprite::Object2D>();
	auto* fixedTetroTilesParent = _bottomBlob->children().last();

	fixedTetroTilesParent->setTransformation(_tetronimoSprite->transformationMatrix());

	for (auto* child = dynamic_cast<tetris_clone::TetronimoRendering::TetrominoObject2D*>(_tetronimoSprite->children().first()); child != nullptr; child = dynamic_cast<tetris_clone::TetronimoRendering::TetrominoObject2D*>(_tetronimoSprite->children().first())) {
		child->globalX = child->x + _te.x();
		child->globalY = child->y + _te.y();
		child->setParent(fixedTetroTilesParent);
	}

	int x[4];
	int y[4];
	auto* fosterChild = dynamic_cast<tetris_clone::TetronimoRendering::TetrominoObject2D*>(fixedTetroTilesParent->children().first());
	for (int i = 0; i < 4; ++i) {
		x[i] = fosterChild->x + _te.x();
		y[i] = fosterChild->y + _te.y();
		fosterChild = dynamic_cast<tetris_clone::TetronimoRendering::TetrominoObject2D*>(fosterChild->nextSibling());
	}

	_tb.placeTetromino(x, y, _te.toTile());

#if 0
	if (!readMode)
		_te = tetris_clone::Tetronimo::simpleSpawn();
	_oldTe = _te;
	_renderer->redoTetronimoRender(_te, globalTexCoords.data(), 8, _testMachine.is<RegState::placement_mode>() /*_placementMode*/);
#endif
}

void MyApplication::transferTetrominos(int size, int startingRow) {
	using TetroOb2d = tetris_clone::TetronimoRendering::TetrominoObject2D;
	for (auto& tetrominoRenderChild : _bottomBlob->children()) {
		RudimentarySprite::Object2D* fosterChildren[4] = { {}, {}, {}, {} };
		int fosterChildrenSize{};
		//packCoordsTogether(tetrominoRenderChild.children().first());
		for (auto* tileChild = dynamic_cast<TetroOb2d*>(tetrominoRenderChild.children().first()); tileChild != nullptr; tileChild = dynamic_cast<TetroOb2d*>(tileChild->nextSibling())) {
			for (int i = 0; i < size; ++i) {
				if (startingRow+i == tileChild->globalY) {
					fosterChildren[fosterChildrenSize++] = tileChild;
				}
			}
		}
		//packCoordsTogether(tetrominoRenderChild.children().first());

		for (int i = 0; i < fosterChildrenSize; ++i) {
			dynamic_cast<RudimentarySprite*>(fosterChildren[i]->features().first())->setVisibility(false);
			fosterChildren[i]->setParent(_renderTilePool);
		}
		//packCoordsTogether(tetrominoRenderChild.children().first());

		if (fosterChildrenSize > 0 and tetrominoRenderChild.children().first()) {
			if (fosterChildrenSize == 4) {} // This should not work; otherwise, logical fault present
			else
				auto silly = subdivideTetros(*tetrominoRenderChild.children().first(), 4 - fosterChildrenSize, *_droppableBlob, size, startingRow);
		}
	}
	if (!_bottomBlob->children().isEmpty()) {
		auto* childi = _bottomBlob->children().first();
		//packCoordsTogether(childi->children().first());
		while (childi and childi->nextSibling()) {
			//packCoordsTogether(childi->nextSibling()->children().first());
			while (childi->nextSibling() and tetrosAboveRow(childi->nextSibling()->children().first(), size, startingRow))
				childi->nextSibling()->setParent(_droppableBlob);
			childi = childi->nextSibling();
		}
		if (tetrosAboveRow(_bottomBlob->children().first()->children().first(), size, startingRow)) 
			_bottomBlob->children().first()->setParent(_droppableBlob);
	}
	//_placementMode = false;
	//_moveBlobMode = true;
#if 1
	if (!_testMachine.is<RegState::dropping>()) {
		_testMachine.toggle<RegState::placement_mode>();
		_testMachine.toggle<RegState::move_blob_mode_start>();
		_testMachine.toggle<RegState::move_blob_mode>();
	}
#else
	_testMachine.toggle<RegState::dropping>();
	_testMachine.toggle<RegState::beginning_drop>();
#endif
	++_i;
}

void MyApplication::transferTetrosBack(int rowsCleared) {
	using TetroOb2d = tetris_clone::TetronimoRendering::TetrominoObject2D;

	for (auto* child = _droppableBlob->children().first(); child != nullptr; child = _droppableBlob->children().first()) {
		for (auto* iChild = dynamic_cast<TetroOb2d*>(child->children().first()); iChild != nullptr; iChild = dynamic_cast<TetroOb2d*>(iChild->nextSibling())) {
			iChild->globalY -= rowsCleared;
		}
		child->setParentKeepTransformation(_bottomBlob);
	}
}

int MyApplication::subdivideTetros(RudimentarySprite::Object2D& src, int n, RudimentarySprite::Object2D& dst, int clearedRowSize, int startingRow) {
	using TetroOb2d = tetris_clone::TetronimoRendering::TetrominoObject2D;
	packCoordsTogether(&src);

	if (!src.nextSibling())
		return 0;

	bool N2disjoint{};
	bool N3disjoint{};

	TetroOb2d* disjointChunk{};

	if (adjacentTiles(dynamic_cast<TetroOb2d*>(&src), dynamic_cast<TetroOb2d*>(src.nextSibling()))) {
		//return 1;  // what is this??
		if (src.nextSibling()->nextSibling()) {
			if (adjacentTiles(dynamic_cast<TetroOb2d*>(src.nextSibling()), dynamic_cast<TetroOb2d*>(src.nextSibling()->nextSibling()))) {
				return 0;
			}
			else {
				N3disjoint = true;
				disjointChunk = dynamic_cast<TetroOb2d*>(src.nextSibling()->nextSibling());
			}
		}
		else {
			return 0;
		}
	}
	else if (src.nextSibling()->nextSibling() ) {
		if (adjacentTiles(dynamic_cast<TetroOb2d*>(src.nextSibling()), dynamic_cast<TetroOb2d*>(src.nextSibling()->nextSibling()))) {
			disjointChunk = dynamic_cast<TetroOb2d*>(src.nextSibling());
		}
		else if (adjacentTiles(dynamic_cast<TetroOb2d*>(&src), dynamic_cast<TetroOb2d*>(src.nextSibling()->nextSibling()))) {
			N2disjoint = true;
			disjointChunk = dynamic_cast<TetroOb2d*>(src.nextSibling());
		}
		else {
			return -1;
		}
	}
	else {
		disjointChunk = dynamic_cast<TetroOb2d*>(src.nextSibling());
		N2disjoint = true;
	}

	int resulty{};
	packCoordsTogether(&src);

	// TODO: this only made to detect a single row chunk removed (needs one more row for proper function) [2020-01-14 18:29:00] CHANGES WERE APPLIED BY NOW. Is this still true??
	if ( ! (disjointChunk->globalY < startingRow + clearedRowSize))
		resulty = divisionOperationLatter(&src, disjointChunk);
	else
		resulty = divisionOperationFormer(&src, disjointChunk);
#if 0
	int traversed{};
	for (auto* tetrominoRenderChild = dynamic_cast<TetroOb2d*>(&src); tetrominoRenderChild != nullptr; tetrominoRenderChild = dynamic_cast<TetroOb2d*>(tetrominoRenderChild->nextSibling())) {

	}
#endif
	return -1;
}

bool MyApplication::adjacentTiles(tetris_clone::TetronimoRendering::TetrominoObject2D* first, tetris_clone::TetronimoRendering::TetrominoObject2D* second)
{
	auto x1 = first->x;
	auto y1 = first->y;
	
	auto x2 = second->x;
	auto y2 = second->y;

	return (y1 == y2 and 
		   (x1 - x2 == 1 or x2 - x1 == 1))
		or 
		   (x1 == x2 and 
		   (y2 - y1 == 1) or (y1 - y2 == 1));
}

int MyApplication::divisionOperationLatter(RudimentarySprite::Object2D* begin, RudimentarySprite::Object2D* startFrom)
{
	using tetro_obj_2d_t = tetris_clone::TetronimoRendering::TetrominoObject2D;
	auto* tileParent = begin->parent();
#if 1
	if (begin == startFrom)
		std::exit(-44);

	auto* newDroppableRenderObj = new tetris_clone::TetronimoRendering::Object2D{_droppableBlob}; // note: no tetro render object
#endif
	newDroppableRenderObj->setTransformation(tileParent->transformationMatrix());

	auto* ptr = startFrom->previousSibling();

	startFrom->setParent(newDroppableRenderObj);

	auto* newlyDroppableTetronimoObj = dynamic_cast<tetris_clone::TetronimoRendering::TetrominoObject2D*>(startFrom);

	while (ptr->nextSibling() and adjacentTiles(newlyDroppableTetronimoObj, dynamic_cast<tetro_obj_2d_t*>(ptr->nextSibling()))) {
		ptr->nextSibling()->setParent(newDroppableRenderObj);
		newlyDroppableTetronimoObj = dynamic_cast<tetro_obj_2d_t*>(newlyDroppableTetronimoObj->nextSibling());
	}
	// TODO: untested...

	return 0;
}

int MyApplication::divisionOperationFormer(RudimentarySprite::Object2D* begin, RudimentarySprite::Object2D* startFrom)
{
	auto* tileParent = begin->parent();
#if 1
	if (begin == startFrom)
		std::exit(-45);

	auto* newDroppableRenderObj = new tetris_clone::TetronimoRendering::Object2D{_droppableBlob}; // note: no tetro render object
#endif
	newDroppableRenderObj->setTransformation(tileParent->transformationMatrix());


	while ( tileParent->children().first() != startFrom ) {
		tileParent->children().first()->setParent(newDroppableRenderObj);
	}
	// TODO: untested...

	return 0;

	return -1;

}
bool MyApplication::tetrosAboveRow(const RudimentarySprite::Object2D* firstChild, int clearedRowSize, int startingRow) const {
	using tetro_obj_t = tetris_clone::TetronimoRendering::TetrominoObject2D;
	if (!firstChild)
		return false;
	do {
		auto localGlobalY = dynamic_cast<const tetro_obj_t*>(firstChild)->globalY;
		auto cmpRow = startingRow + clearedRowSize;
		if (dynamic_cast<const tetro_obj_t*>(firstChild)->globalY < cmpRow) {
			return false;
		}
	} while (firstChild = firstChild->nextSibling());
	return true;
}


MyApplication::pointer_things MyApplication::packCoordsTogether(RudimentarySprite::Object2D* ptr) const {
	using tetro_obj_t = tetris_clone::TetronimoRendering::TetrominoObject2D;

	auto* curr = ptr;
//	auto* converted = dynamic_cast<tetro_obj_t*>(curr);

	std::pair<int, int> arraya[4];

	int rowey = _tb[0];

	for (int i = 0; i < 4; ++i) {
		if (!curr)
			arraya[i] = { -1, -1 };
		else {
			arraya[i] = { dynamic_cast<tetro_obj_t*>(curr)->globalX, dynamic_cast<tetro_obj_t*>(curr)->globalY };
			curr = dynamic_cast<tetro_obj_t*>(curr->nextSibling());
		}
	}

	return &arraya;
}

void MyApplication::resetRenderer( bool makeVisible) {
	_renderer->redoTetronimoRender(_te, globalTexCoords.data(), 8, makeVisible);
}

void MyApplication::debugPrint01_(const std::tuple<char, char, int, int> args) {
	auto [a, b, c, d] = args;
	const char astr[2] = { a, '\0' };
	const char bstr[2] = { b, '\0' };
	_out << astr << " " << bstr << " " << c << " " << d << "\n";
}


#if 0
void MyApplication::mousePressEvent(MouseEvent& event) {
	if (event.button() != MouseEvent::Button::Left) return;

	event.setAccepted();
}

void MyApplication::mouseReleaseEvent(MouseEvent& event) {
	using namespace Math::Literals;
	_color = Color3::fromHsv({ _color.hue() + 50.0_degf, 1.0f, 1.0f });

	event.setAccepted();

	redraw();
}

void MyApplication::mouseMoveEvent(MouseMoveEvent& event) {
	if (!(event.buttons() & MouseMoveEvent::Button::Left)) return;

	Vector2 delta = 3.0f * Vector2{ event.relativePosition() } / Vector2{ windowSize() };

	_transformation = Matrix4::rotationX(Rad{ delta.y() }) *
		              _transformation *
		              Matrix4::rotationY(Rad{ delta.x() });

	event.setAccepted();
	redraw();
}
#endif

MAGNUM_APPLICATION_MAIN(MyApplication)
