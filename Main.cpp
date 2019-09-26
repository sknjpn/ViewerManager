#include "ViewerManager.h"
#include "Viewer.h"

class TitleViewer
	: public Viewer
{
	Font	m_font;
	Vec2	m_m_catPos;
	Texture m_cat;
	Color	m_color;

public:
	void Init() override
	{
		// 背景を水色にする
		SetBackgroundColor(ColorF(0.8, 0.9, 1.0));

		// 大きさ 60 のフォントを用意
		m_font = Font(60);

		// 猫のテクスチャを用意
		m_cat = Texture(Emoji(U"🐈"));

		// 猫の座標
		m_m_catPos.set(400, 450);
		m_color = Palette::White;

		SetDrawRect(600, 600);
	}

	void update() override
	{
		// テキストを画面の中心に描く
		m_font(U"Hello, Siv3D!🐣").drawAt(Scene::Center(), Palette::Black);

		// 大きさをアニメーションさせて猫を表示する
		m_cat.resized(100 + Periodic::Sine0_1(1s) * 20).drawAt(m_m_catPos, m_color);

		// マウスカーソルに追従する半透明の赤い円を描く
		Circle(Cursor::Pos(), 40).draw(ColorF(1, 0, 0, 0.5));

		// [A] キーが押されたら
		if (KeyA.down())
		{
			// Hello とデバッグ表示する
			Print << U"Hello!";
		}

	}

	void	SetColor(const Color& color) { m_color = color; }
	void	SetCatPos(const Vec2& pos) { m_m_catPos = pos; }
};

class ButtonViewer
	: public Viewer
{
public:
	void Init()
	{
		SetDrawRect(600, 0, 200, 300);
		SetBackgroundColor(Palette::Gray);
	}

	void update()
	{
		if (SimpleGUI::Button(U"White", Vec2(10, 10), 180))
		{
			g_viewerManagerPtr->getViewer<TitleViewer>()->SetColor(Palette::White);
		}
		if (SimpleGUI::Button(U"Red", Vec2(10, 50), 180))
		{
			g_viewerManagerPtr->getViewer<TitleViewer>()->SetColor(Palette::Red);
		}
		if (SimpleGUI::Button(U"Green", Vec2(10, 90), 180))
		{
			g_viewerManagerPtr->getViewer<TitleViewer>()->SetColor(Palette::Green);
		}
		if (SimpleGUI::Button(U"Blue", Vec2(10, 130), 180))
		{
			g_viewerManagerPtr->getViewer<TitleViewer>()->SetColor(Palette::Blue);
		}
	}
};

class TeleportViewer
	: public Viewer
{
public:
	void Init()
	{
		SetDrawRect(600, 300, 200, 300);
		SetBackgroundColor(Palette::Gray);
	}

	void update()
	{
		// ボタンが押されたら
		if (SimpleGUI::Button(U"Move the cat", Vec2(10, 10), 180))
		{
			// 猫の座標を画面内のランダムな位置に移動する
			g_viewerManagerPtr->getViewer<TitleViewer>()->SetCatPos(RandomVec2(Scene::Rect()));
		}
	}
};

void Main()
{
	g_viewerManagerPtr = MakeUnique<ViewerManager>();
	g_viewerManagerPtr->makeViewer<TitleViewer>();
	g_viewerManagerPtr->makeViewer<ButtonViewer>();
	g_viewerManagerPtr->makeViewer<TeleportViewer>();

	while (System::update())
	{
		g_viewerManagerPtr->update();
	}
}
