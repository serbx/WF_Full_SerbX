#pragma once

enum ItemType
{
	ITEM_BOOL = 0,
	ITEM_NUMBER,
	ITEM_TEXT,
	ITEM_TEXT_ARRAY
};

struct OptionValue
{
	std::vector<std::string> TextArray;
	size_t Selected;
};

union ItemValue
{
	std::string* Text;
	bool* Check;
	struct {
		int32_t* Number;
		int32_t Minimum;
		int32_t Maximum;
	} Numeric;

	OptionValue* Options;

	ItemValue()
	{
		Check = nullptr;
	}
	~ItemValue()
	{
		Check = nullptr;
	}
};

struct Item
{
	std::string Name;
	ItemValue Value;
	ItemType Type;
};

class Menu;

class MenuGroup
{
protected:
	std::string m_name;
	bool m_minimized;
	float m_height;
	std::vector<Item> m_items;
	size_t m_selected_item;
	uint32_t m_color, m_title_color, m_title_background_color, m_items_color, m_selected_items_color;

	Font m_title_font, m_item_font;
public:

	MenuGroup(const std::string name="Menu Group", const DWORD color = 0x5FC0C0C0, Font title_font=Font("Helvetica Neue, Roboto, Arial, Droid Sans, sans - serif", 13, Gdiplus::FontStyle::FontStyleBold))
		: m_name(name), m_color(color), m_title_font(title_font), m_minimized(false)
	{
		m_selected_item = 0;
		m_title_color = 0xFFB8B3B2;
		m_items_color = 0xFF000000;
		m_title_background_color = 0xFF424242;
		m_selected_items_color = 0xFF8D6E63;

		m_item_font = Font("Helvetica Neue, Roboto, Arial, Droid Sans, sans - serif", 12);
	}
	~MenuGroup() {}

	void Render(Renderer* r, float x, float y, float w);
	void Navigate();

	std::string getName() const { return m_name; }
	bool isMinimized() const { return m_minimized; }
	float getCalculatedHeight() const { return m_height; }
	size_t getSelectedItemIndex() const { return m_selected_item; }
	uint32_t getColor() const { return m_color; }
	uint32_t getTitleColor() const { return m_title_color; }
	uint32_t getItemColor() const { return m_items_color; }
	uint32_t getSelectedItemColor() const { return m_selected_items_color; }

	std::vector<Item>& Items() { return m_items; }
	Font getTitleFont() const { return m_title_font; }
	Font getItemFont() const { return m_title_font; }

	void setName(std::string val) { m_name = val; }
	void toggleMinimized() { m_minimized = !m_minimized; }
	void setCalculatedHeight(float val) { m_height = val; }
	void setSelectedItemIndex(size_t val) { m_selected_item = val; }
	void setColor(uint32_t val) { m_color = val; }
	void setTitleColor(uint32_t val) { m_title_color = val; }
	void setItemColor(uint32_t val) { m_items_color = val; }
	void setSelectedItemColor(uint32_t val) { m_selected_items_color = val; }

	void setTitleFont(const Font& font) { m_title_font = font; }
	void setItemFont(const Font& font) { m_title_font = font; }

	void Add(const Item& val) { m_items.push_back(val); }

	void AddBool(const std::string name, bool* value)
	{
		Item t;
		t.Name = name;
		t.Value.Check = value;
		t.Type = ITEM_BOOL;
		Add(t);
	}
	void AddText(const std::string name, std::string* value)
	{
		Item t;
		t.Name = name;
		t.Value.Text = value;
		t.Type = ITEM_TEXT;
		Add(t);
	}
	void AddTextArray(const std::string name, OptionValue* value)
	{
		Item t;
		t.Name = name;
		t.Value.Options = value;
		t.Type = ITEM_TEXT_ARRAY;
		Add(t);
	}
	void AddNumber(const std::string name, int32_t* value, int32_t maximum = 5, int32_t minimum = 0)
	{
		Item t;
		t.Name = name;
		t.Value.Numeric.Number = value;
		t.Value.Numeric.Minimum = minimum;
		t.Value.Numeric.Maximum = maximum;
		t.Type = ITEM_NUMBER;
		Add(t);
	}
	friend class Menu;
};

class Menu
{
protected:
	std::string m_title;
	float m_x, m_y, m_w, m_height;
	std::vector<MenuGroup> m_groups;
	size_t m_selected_group;
	Font m_title_font;
	uint32_t m_color, m_title_color, m_title_background_color;

public:
	bool m_minimized;
	Menu(const std::string title = "", const DWORD color = 0xCFD9D8D7, Font title_font = Font("Helvetica Neue, Roboto, Arial, Droid Sans, sans - serif", 15, Gdiplus::FontStyle(Gdiplus::FontStyleBold)))
		: m_title(title), m_color(color), m_title_font(title_font), m_minimized(false), m_selected_group(0)
	{
		m_title_color = 0xFFE7E7E7;
		m_title_background_color = 0xFF4E342E;
		m_x = m_y = 100.f;
		m_w = 200.f;
	}

	~Menu() {}

	std::string getTitle() const { return m_title; }
	bool isMinimized() const { return m_minimized; }
	float getCalculatedHeight() const { return m_height; }
	uint32_t getColor() const { return m_color; }
	uint32_t getTitleColor() const { return m_title_color; }
	uint32_t getTitleBackgroundColor() const { return m_title_background_color; }
	float getX() const { return m_x; }
	float getY() const { return m_y; }
	float getWidth() const { return m_w; }

	std::vector<MenuGroup>& Groups() { return m_groups; }

	Font getTitleFont() const { return m_title_font; }

	void setTitle(std::string val) { m_title = val; }
	void toggleMinimized() { m_minimized = !m_minimized; }
	void setCalculatedHeight(float val) { m_height = val; }
	void setSelectedGroupIndex(size_t val) { m_selected_group = val; }
	void setColor(uint32_t val) { m_color = val; }
	void setTitleColor(uint32_t val) { m_title_color = val; }
	void setTitleBackgroundColor(uint32_t val) { m_title_background_color = val; }
	void setX(float val) { m_x = val; }
	void setY(float val) { m_y = val; }
	void setWidth(float val) { m_w = val; }

	void Add(MenuGroup& grp)
	{
		if (m_groups.size() != 0)
			grp.setSelectedItemIndex(-1);
		m_groups.push_back(grp);
	}

	inline void Render(Renderer* r) { Render(r, m_x, m_y, m_w); }
	void Render(Renderer* r, float x, float y, float w);
	void Navigate();
};

