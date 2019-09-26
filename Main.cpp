#include "ViewerManager.h"
#include "Viewer.h"

class TitleViewer
	: public Viewer
{
	Font font;
	Vec2 catPos;
	Texture cat;

public:
	void Init() override
	{
		// 背景を水色にする
		SetBackgroundColor(ColorF(0.8, 0.9, 1.0));

		// 大きさ 60 のフォントを用意
		font = Font(60);

		// 猫のテクスチャを用意
		cat = Texture(Emoji(U"🐈"));

		// 猫の座標
		catPos.set(640, 450);
	}

	void Update() override
	{
		// テキストを画面の中心に描く
		font(U"Hello, Siv3D!🐣").drawAt(Scene::Center(), Palette::Black);

		// 大きさをアニメーションさせて猫を表示する
		cat.resized(100 + Periodic::Sine0_1(1s) * 20).drawAt(catPos);

		// マウスカーソルに追従する半透明の赤い円を描く
		Circle(Cursor::Pos(), 40).draw(ColorF(1, 0, 0, 0.5));

		// [A] キーが押されたら
		if (KeyA.down())
		{
			// Hello とデバッグ表示する
			Print << U"Hello!";
		}

		// ボタンが押されたら
		if (SimpleGUI::Button(U"Move the cat", Vec2(600, 20)))
		{
			// 猫の座標を画面内のランダムな位置に移動する
			catPos = RandomVec2(Scene::Rect());
		}
	}
};

void Main()
{
	g_viewerManagerPtr = MakeUnique<ViewerManager>();
	g_viewerManagerPtr->MakeViewer<TitleViewer>();

	while (System::Update())
	{
		g_viewerManagerPtr->Update();
	}
}
