// Copyright (c) 2025 Inan Evin

#ifndef VEKT_HPP
#define VEKT_HPP

#include <memory>
#include <assert.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <utility> // For std::swap, std::move

#ifndef VEKT_STRING
#include <string>
#endif

#ifndef VEKT_VARIANT
#include <variant>
#endif

#define VEKT_INLINE inline
#define VEKT_API	extern

#undef min
#undef max

namespace vekt
{

	////////////////////////////////////////////////////////////////////////////////
	// :: COMMON DEFINES
	////////////////////////////////////////////////////////////////////////////////

#define M_PI	  3.14159265358979f
#define DEG_2_RAD 0.0174533f
#ifndef VEKT_USER_DATA_SIZE
#define VEKT_USER_DATA_SIZE 1024
#endif

#if defined _MSC_VER && !__INTEL_COMPILER
#define ALIGNED_MALLOC(SZ, ALIGN) _aligned_malloc(SZ, ALIGN)
#define ALIGNED_FREE(PTR)		  _aligned_free(PTR);
#else
#include <stdlib.h>
#define ALIGNED_MALLOC(SZ, ALIGN) std::aligned_alloc(ALIGN, SZ)
#define ALIGNED_FREE(PTR)		  std::free(PTR)
#endif

#define MEMMOVE(...) memmove(__VA_ARGS__) ALIGNED_MALLOC
#define REALLOC(...) realloc(__VA_ARGS__)
#define MEMCPY(...)	 memcpy(__VA_ARGS__)
#define ASSERT(...)	 assert(__VA_ARGS__)
#define MEMSET(...)	 memset(__VA_ARGS__)

#ifndef VEKT_STRING

#ifdef VEKT_STRING_CSTR
#define VEKT_STRING const char*
#else
#define VEKT_STRING std::string
#endif
#endif

#ifndef VEKT_VARIANT
#define VEKT_VARIANT std::variant
#endif

	////////////////////////////////////////////////////////////////////////////////
	// :: LOGS & CONFIGS
	////////////////////////////////////////////////////////////////////////////////

	enum class log_verbosity
	{
		info,
		warning,
		error
	};

	typedef void (*log_callback)(log_verbosity, const char*, ...);
	struct config_data
	{
		log_callback on_log		  = nullptr;
		unsigned int atlas_width  = 1024;
		unsigned int atlas_height = 1024;
	};

	extern config_data config;

#define V_LOG(...)                                                                                                                                                                                                                                                 \
	if (vekt::config.on_log)                                                                                                                                                                                                                                       \
	vekt::config.on_log(vekt::log_verbosity::info, __VA_ARGS__)
#define V_ERR(...)                                                                                                                                                                                                                                                 \
	if (vekt::config.on_log)                                                                                                                                                                                                                                       \
	vekt::config.on_log(vekt::log_verbosity::error, __VA_ARGS__)
#define V_WARN(...)                                                                                                                                                                                                                                                \
	if (vekt::config.on_log)                                                                                                                                                                                                                                       \
	vekt::config.on_log(vekt::log_verbosity::warning, __VA_ARGS__)

#define MALLOC(SZ) malloc(SZ)
#define FREE(X)	   free(X)

	////////////////////////////////////////////////////////////////////////////////
	// :: COMMON CONTAINERS
	////////////////////////////////////////////////////////////////////////////////

	typedef int id;

	template <typename T> class vector
	{
	public:
		using iterator								   = T*;
		using const_iterator						   = const T*;
		static constexpr unsigned int initial_capacity = 4;

		vector(){};
		vector(const vector<T>& other)
		{
			if (other.empty())
			{
				return;
			}

			_capacity = other._capacity;
			_elements = reinterpret_cast<T*>(MALLOC(_capacity * sizeof(T)));
			if (!_elements)
				throw std::bad_alloc();

			for (unsigned int i = 0; i < other._count; ++i)
			{
				new (&_elements[i]) T(other._elements[i]);
			}
			_count = other._count;
		}

		vector<T>& operator=(const vector<T>& other)
		{
			if (this == &other)
			{
				return *this;
			}

			clear();

			if (other.empty())
			{
				return *this;
			}

			_capacity = other._capacity;
			_elements = reinterpret_cast<T*>(MALLOC(_capacity * sizeof(T)));
			if (!_elements)
			{
				throw std::bad_alloc();
			}

			_count = other._count;
			MEMCPY(_elements, other._elements, _count * sizeof(T));

			return *this;
		}

		~vector()
		{
			clear();
		}

		inline void push_back(const T& elem)
		{
			check_grow();
			new (&_elements[_count]) T(elem);
			_count++;
		}

		inline void push_back(T&& elem)
		{
			check_grow();
			new (&_elements[_count]) T(std::move(elem));
			_count++;
		}

		inline void pop_back()
		{
			remove_index(_count - 1);
		}

		inline void increment_back()
		{
			const bool req_place = _count >= _capacity;
			check_grow();
			if (req_place)
				new (&_elements[_count]) T();
			_count++;
		}

		inline T& get_back()
		{
			return _elements[_count - 1];
		}

		inline void remove_index(unsigned int index)
		{
			if (index >= _count)
			{
				return;
			}
			_elements[index].~T();

			if (index < _count - 1)
			{
				for (unsigned int i = index; i < _count - 1; ++i)
					_elements[i] = std::move(_elements[i + 1]);
			}
			_count--;
		}

		inline void remove(T& elem)
		{
			for (unsigned int i = 0; i < _count; ++i)
			{
				if (_elements[i] == elem)
				{
					remove_index(i);
					return;
				}
			}
		}

		inline void remove(iterator it)
		{
			remove(*it);
		}

		inline void clear()
		{
			for (unsigned int i = 0; i < _count; ++i)
				_elements[i].~T();

			if (_elements)
			{
				FREE(_elements);
			}
			_elements = nullptr;
			_count	  = 0;
			_capacity = 0;
		}

		inline void reserve(unsigned int new_capacity)
		{
			if (new_capacity <= _capacity)
			{
				return;
			}

			T* new_elements = (T*)MALLOC(new_capacity * sizeof(T));
			if (!new_elements)
			{
				throw std::bad_alloc();
			}

			for (unsigned int i = 0; i < _count; ++i)
			{
				new (&new_elements[i]) T(std::move(_elements[i]));
				_elements[i].~T();
			}

			for (unsigned int i = _capacity; i < new_capacity; i++)
			{
				new (&new_elements[i]) T();
			}

			if (_elements)
			{
				FREE(_elements);
			}

			_elements = new_elements;
			_capacity = new_capacity;
		}

		inline void resize(unsigned int sz, bool call_dest = true)
		{
			// Handle destructors if shrinking.
			if (call_dest && sz < _count)
			{
				for (unsigned int i = sz; i < _count; ++i)
					_elements[i].~T();
			}

			if (sz > _capacity)
			{
				T* new_elements = (T*)MALLOC(sz * sizeof(T));
				if (!new_elements)
					throw std::bad_alloc();

				// Move existing elements
				for (unsigned int i = 0; i < _count; ++i)
				{
					new (&new_elements[i]) T(std::move(_elements[i]));
					_elements[i].~T();
				}

				FREE(_elements);
				_elements = new_elements;
				_capacity = sz;
			}

			// If growing, the new elements are default-constructed
			if (sz > _count)
			{
				for (unsigned int i = _count; i < sz; ++i)
					new (&_elements[i]) T();
			}
			_count = sz;
		}

		inline void resize_explicit(unsigned int sz)
		{
			if (sz > _capacity)
			{
				T* new_elements = (T*)MALLOC(sz * sizeof(T));
				if (!new_elements)
					throw std::bad_alloc();

				// Move existing elements
				for (unsigned int i = 0; i < _count; ++i)
				{
					new (&new_elements[i]) T(std::move(_elements[i]));
					_elements[i].~T();
				}

				FREE(_elements);
				_elements = new_elements;
				_capacity = sz;
			}

			_count = sz;
		}

		inline void reverse()
		{
			std::reverse(begin(), end());
		}

		inline int index_of(const T& t) const
		{
			for (int i = 0; i < _count; i++)
				if (t == _elements[i])
					return i;
			return -1;
		};

		inline iterator find(T& t)
		{
			for (unsigned int i = 0; i < _count; i++)
			{
				if (t == _elements[i])
					return _elements + i;
			}
			return end();
		};

		inline iterator find(std::function<bool(const T&)> predicate)
		{
			for (unsigned int i = 0; i < _count; i++)
			{
				if (predicate(_elements[i]))
					return _elements + i;
			}
			return end();
		};

		inline T* data() const
		{
			return _elements;
		}

		T& operator[](unsigned int index)
		{
			return _elements[index];
		}
		const T& operator[](unsigned int index) const
		{
			return _elements[index];
		}
		inline bool empty() const
		{
			return _count == 0;
		}
		inline unsigned int size() const
		{
			return _count;
		}

		inline iterator begin()
		{
			return _elements;
		}
		inline const_iterator begin() const
		{
			return _elements;
		}
		inline const_iterator cbegin() const
		{
			return _elements;
		}
		inline iterator end()
		{
			return _elements + _count;
		}
		inline const_iterator end() const
		{
			return _elements + _count;
		}
		inline const_iterator cend() const
		{
			return _elements + _count;
		}

	private:
		inline void check_grow()
		{
			if (_count >= _capacity)
			{
				unsigned int new_capacity = (_capacity == 0) ? initial_capacity : _capacity * 2;
				reserve(new_capacity);
			}
		}

	private:
		T*			 _elements = nullptr;
		unsigned int _count	   = 0;
		unsigned int _capacity = 0;
	};

	////////////////////////////////////////////////////////////////////////////////
	// :: VECTORS & MATH
	////////////////////////////////////////////////////////////////////////////////

	class math
	{
	public:
		template <typename T> static inline T max(T a, T b)
		{
			return a > b ? a : b;
		}
		template <typename T> static inline T min(T a, T b)
		{
			return a < b ? a : b;
		}
		static inline float equals(float a, float b, float eps = 0.0001f)
		{
			return a > b - eps && a < b + eps;
		}
		static inline float cos(float x)
		{
			return std::cos(x);
		}
		static inline float sin(float x)
		{
			return std::sin(x);
		}
		static inline float lerp(float a, float b, float t)
		{
			return a + (b - a) * t;
		}
		static inline float ceilf(float f)
		{
			return std::ceilf(f);
		}
		static inline float remap(float val, float from_low, float from_high, float to_low, float to_high)
		{
			return to_low + (val - from_low) * (to_high - to_low) / (from_high - from_low);
		}
	};

#ifndef VEKT_VEC2

	struct vec2
	{
		float x = 0.0f;
		float y = 0.0f;

		vec2 operator+(const vec2& other) const
		{
			vec2 v = {};
			v.x	   = x + other.x;
			v.y	   = y + other.y;
			return v;
		}

		vec2 operator*(float f) const
		{
			vec2 v = *this;
			v.x *= f;
			v.y *= f;
			return v;
		}

		vec2 operator-(const vec2& other) const
		{
			vec2 v = *this;
			v.x -= other.x;
			v.y -= other.y;
			return v;
		}

		vec2 operator/=(const vec2& other)
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}

		vec2 operator/=(float f)
		{
			x /= f;
			y /= f;
			return *this;
		}

		vec2 operator*=(float f)
		{
			x *= f;
			y *= f;
			return *this;
		}

		vec2 operator+=(const vec2& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		bool operator==(const vec2& other) const
		{
			return math::equals(x, other.x) && math::equals(y, other.y);
		}

		inline void normalize()
		{
			const float s = mag();
			x /= s;
			y /= s;
		}

		inline vec2 normalized()
		{
			vec2 v = *this;
			v.normalize();
			return v;
		}

		inline float mag()
		{
			return sqrt(x * x + y * y);
		}
		inline float mag2()
		{
			return x * x + y * y;
		}
	};

#define VEKT_VEC2 vec2
#endif

#ifndef VEKT_VEC4

	struct vec4
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 0.0f;

		bool equals(const vec4& other, float eps = 0.1f) const
		{
			return math::equals(x, other.x, eps) && math::equals(y, other.y, eps) && math::equals(z, other.z, eps) && math::equals(w, other.w, eps);
		}
		bool is_point_inside(float _x, float _y) const
		{
			return _x >= x && _x <= x + z && _y >= y && _y <= y + w;
		}

		static inline vec4 lerp(const vec4& a, const vec4& b, float t)
		{
			return vec4(math::lerp(a.x, b.x, t), math::lerp(a.y, b.y, t), math::lerp(a.z, b.z, t), math::lerp(a.w, b.w, t));
		}

		bool operator==(const vec4& other) const
		{
			return math::equals(x, other.x) && math::equals(y, other.y) && math::equals(z, other.z) && math::equals(w, other.w);
		}

		vec4 operator+(const vec4& other) const
		{
			vec4 v = {};
			v.x	   = x + other.x;
			v.y	   = y + other.y;
			v.z	   = z + other.z;
			v.w	   = w + other.w;
			return v;
		}

		vec4 operator-(const vec4& other) const
		{
			vec4 v = {};
			v.x	   = x - other.x;
			v.y	   = y - other.y;
			v.z	   = z - other.z;
			v.w	   = w - other.w;
			return v;
		}

		vec4 operator*(float f) const
		{
			vec4 v = *this;
			v.x *= f;
			v.y *= f;
			v.z *= f;
			v.w *= f;
			return v;
		}
	};

#define VEKT_VEC4 vec4
#endif

	////////////////////////////////////////////////////////////////////////////////
	// :: INPUT
	////////////////////////////////////////////////////////////////////////////////

	enum class input_event_type
	{
		pressed,
		released,
		repeated,
	};

	enum class input_event_result
	{
		handled,
		not_handled,
	};

	enum class input_event_phase
	{
		tunneling,
		bubbling,
	};

	struct mouse_event
	{
		input_event_type type	  = input_event_type::pressed;
		int				 button	  = 0;
		VEKT_VEC2		 position = VEKT_VEC2();
	};

	struct mouse_wheel_event
	{
		float amount = 0.0f;
	};

	struct key_event
	{
		input_event_type type	   = input_event_type::pressed;
		int				 key	   = 0;
		int				 scan_code = 0;
	};

	////////////////////////////////////////////////////////////////////////////////
	// :: WIDGET UTILS
	////////////////////////////////////////////////////////////////////////////////

	enum size_flags
	{
		sf_x_relative		= 1 << 0,
		sf_y_relative		= 1 << 1,
		sf_x_abs			= 1 << 2,
		sf_y_abs			= 1 << 3,
		sf_x_copy_y			= 1 << 4,
		sf_y_copy_x			= 1 << 5,
		sf_x_total_children = 1 << 6,
		sf_x_max_children	= 1 << 7,
		sf_y_total_children = 1 << 8,
		sf_y_max_children	= 1 << 9,
		sf_x_fill			= 1 << 10,
		sf_y_fill			= 1 << 11,
		sf_custom_pass		= 1 << 12,
	};

	enum pos_flags
	{
		pf_x_relative		= 1 << 0,
		pf_y_relative		= 1 << 1,
		pf_x_abs			= 1 << 2,
		pf_y_abs			= 1 << 3,
		pf_x_anchor_center	= 1 << 4,
		pf_x_anchor_end		= 1 << 5,
		pf_y_anchor_center	= 1 << 6,
		pf_y_anchor_end		= 1 << 7,
		pf_child_pos_row	= 1 << 8,
		pf_child_pos_column = 1 << 9,
		pf_custom_pass		= 1 << 10,
	};

	enum class helper_pos_type
	{
		absolute,
		relative,
	};

	enum class helper_size_type
	{
		absolute,
		relative,
		fill,
		max_children,
		total_children,
		copy_other,
	};

	enum class helper_anchor_type
	{
		start,
		center,
		end
	};

	enum class direction
	{
		horizontal,
		vertical
	};

	struct margins
	{
		float top	 = 0.0f;
		float bottom = 0.0f;
		float left	 = 0.0f;
		float right	 = 0.0f;
	};

	////////////////////////////////////////////////////////////////////////////////
	// :: WIDGET GFX
	////////////////////////////////////////////////////////////////////////////////

	enum class gfx_type
	{
		none,
		filled_rect,
		stroke_rect,
		text,
	};

	enum gfx_flags
	{
		gfx_is_rect			 = 1 << 0,
		gfx_is_stroke		 = 1 << 1,
		gfx_is_text			 = 1 << 2,
		gfx_is_text_cached	 = 1 << 4,
		gfx_has_stroke		 = 1 << 4,
		gfx_has_aa			 = 1 << 5,
		gfx_has_second_color = 1 << 6,
		gfx_has_rounding	 = 1 << 7,
		gfx_clip_children	 = 1 << 8,
		gfx_invisible		 = 1 << 9,
		gfx_custom_pass		 = 1 << 10,
	};

	struct stroke_props
	{
		VEKT_VEC4	 color	   = {};
		unsigned int thickness = 0;
	};

	struct aa_props
	{
		unsigned int thickness = 0;
	};

	struct second_color_props
	{
		VEKT_VEC4 color		= VEKT_VEC4(1, 1, 1, 1);
		direction direction = direction::horizontal;
	};

	struct rounding_props
	{
		float		 rounding = 0.0f;
		unsigned int segments = 0;
	};
	struct font;

	struct text_props
	{
		VEKT_STRING	  text	  = "";
		font*		  font	  = nullptr;
		unsigned char spacing = 0;
		float		  scale	  = 1.0f;
		uint64_t	  hash	  = 0;
	};

	struct widget_gfx
	{
		VEKT_VEC4	   color	  = VEKT_VEC4(1, 1, 1, 1);
		void*		   user_data  = nullptr;
		unsigned int   draw_order = 0;
		unsigned short flags	  = 0;
	};

	////////////////////////////////////////////////////////////////////////////////
	// :: WIDGET
	////////////////////////////////////////////////////////////////////////////////
	class builder;

	typedef void			   (*widget_func)(builder* b, id widget);
	typedef input_event_result (*mouse_func)(builder* b, id widget, const mouse_event& ev, input_event_phase phase);
	typedef input_event_result (*key_func)(builder* b, id widget, const key_event& ev);
	typedef input_event_result (*wheel_func)(builder* b, id widget, const mouse_wheel_event& ev);
	typedef void			   (*drag_func)(builder* b, id widget, const VEKT_VEC2& mouse, const VEKT_VEC2& mouse_delta);

	////////////////////////////////////////////////////////////////////////////////
	// :: VERTICES
	////////////////////////////////////////////////////////////////////////////////

	struct vertex
	{
		VEKT_VEC2 pos;
		VEKT_VEC2 uv;
		VEKT_VEC4 color;
	};

	typedef unsigned short index;

	////////////////////////////////////////////////////////////////////////////////
	// :: BUILDER
	////////////////////////////////////////////////////////////////////////////////

	struct draw_buffer
	{
		void*		 user_data	   = nullptr;
		font*		 used_font	   = nullptr;
		VEKT_VEC4	 clip		   = VEKT_VEC4();
		vertex*		 vertex_start  = nullptr;
		index*		 index_start   = nullptr;
		unsigned int draw_order	   = 0;
		unsigned int vertex_count  = 0;
		unsigned int index_count   = 0;
		unsigned int _max_vertices = 0;
		unsigned int _max_indices  = 0;

		inline void add_vertex(const vertex& vtx)
		{
			ASSERT(vertex_count < _max_vertices);
			vertex_start[vertex_count] = vtx;
			vertex_count++;
		}

		inline vertex& add_get_vertex()
		{
			ASSERT(vertex_count < _max_vertices);
			const unsigned int idx = vertex_count;
			vertex_count++;
			return vertex_start[idx];
		}

		inline vertex* add_get_vertex(unsigned int count)
		{
			ASSERT(vertex_count + count < _max_vertices);
			const unsigned int idx = vertex_count;
			vertex_count += count;
			return vertex_start + idx;
		}

		inline void add_index(index idx)
		{
			ASSERT(index_count < _max_indices);
			index_start[index_count] = idx;
			index_count++;
		}

		inline index* add_get_index(unsigned int count)
		{
			ASSERT(index_count + count < _max_indices);
			const unsigned int idx = index_count;
			index_count += count;
			return index_start + idx;
		}
	};

	typedef std::function<void(const draw_buffer& db)> draw_callback;

	class theme
	{
	public:
		static VEKT_VEC4 color_item_bg;
		static VEKT_VEC4 color_item_hover;
		static VEKT_VEC4 color_item_press;
		static VEKT_VEC4 color_panel_bg;
		static VEKT_VEC4 color_divider;
		static VEKT_VEC4 color_item_outline;
		static VEKT_VEC4 color_item_fg;
		static float	 item_height;
		static float	 item_spacing;
		static float	 indent_horizontal;
		static float	 margin_horizontal;
		static float	 margin_vertical;
		static float	 border_thickness;
		static float	 outline_thickness;
	};

	struct hover_callback
	{
		widget_func	  on_hover_begin = nullptr;
		widget_func	  on_hover_end	 = nullptr;
		unsigned char is_hovered	 = 0;
	};

	struct mouse_callback
	{
		mouse_func on_mouse		  = nullptr;
		wheel_func on_mouse_wheel = nullptr;
	};

	struct key_callback
	{
		key_func on_key = nullptr;
	};

	struct widget_meta
	{
		id		   parent = -1;
		vector<id> children;
	};

	struct size_props
	{
		margins		   child_margins = margins();
		VEKT_VEC2	   size			 = VEKT_VEC2();
		float		   spacing		 = 0.0f;
		unsigned short flags		 = 0;
	};

	struct size_result
	{
		VEKT_VEC2 size = VEKT_VEC2();
	};

	struct pos_result
	{
		VEKT_VEC2 pos = VEKT_VEC2();
	};

	struct pos_props
	{
		VEKT_VEC2	   pos			 = VEKT_VEC2();
		float		   scroll_offset = 0.0f;
		unsigned short flags		 = 0;
	};

	struct custom_passes
	{
		widget_func custom_pos_pass	 = nullptr;
		widget_func custom_size_pass = nullptr;
		widget_func custom_draw_pass = nullptr;
	};

	struct text_cache
	{
		uint64_t	 hash	   = 0;
		unsigned int vtx_start = 0;
		unsigned int idx_start = 0;
		unsigned int vtx_count = 0;

		static inline uint64_t hash_combine_64(uint64_t a, uint64_t b)
		{
			return a ^ (b + 0x9e3779b97f4a7c15ull + (a << 12) + (a >> 4));
		}

		static inline uint64_t hash_text_props(const vekt::text_props& text, const VEKT_VEC4& color)
		{
			uint64_t h = std::hash<VEKT_STRING>{}(text.text);
			h		   = hash_combine_64(h, std::hash<void*>{}(text.font));
			h		   = hash_combine_64(h, std::hash<float>{}(text.scale));
			h		   = hash_combine_64(h, std::hash<unsigned char>{}(text.spacing));
			h		   = hash_combine_64(h, std::hash<float>{}(color.x));
			h		   = hash_combine_64(h, std::hash<float>{}(color.y));
			h		   = hash_combine_64(h, std::hash<float>{}(color.z));
			h		   = hash_combine_64(h, std::hash<float>{}(color.w));
			return h;
		}
	};

	class builder
	{

	public:
		struct rect_props
		{
			const widget_gfx& gfx;
			const VEKT_VEC2&  min;
			const VEKT_VEC2&  max;
			bool			  use_hovered;
			bool			  use_pressed;
			VEKT_VEC4		  color_start;
			VEKT_VEC4		  color_end;
			direction		  color_direction;
			id				  widget_id = 0;
			bool			  multi_color;
		};

	private:
	public:
		struct input_layer
		{
			unsigned int priority = 0;
			id			 root	  = -1;
			bool		 operator==(const input_layer& other) const
			{
				return priority == other.priority && root == other.root;
			}
		};

		struct init_config
		{
			unsigned int widget_count				 = 1024;
			size_t		 vertex_buffer_sz			 = 1024 * 1024;
			size_t		 index_buffer_sz			 = 1024 * 1024;
			size_t		 text_cache_vertex_buffer_sz = 1024 * 1024;
			size_t		 text_cache_index_buffer_sz	 = 1024 * 1024;
			size_t		 buffer_count				 = 10;
		};

		builder()					  = default;
		builder(const builder& other) = delete;
		~builder()
		{
		}

		void				init(const init_config& conf);
		void				uninit();
		void				build_begin(const VEKT_VEC2& screen_size);
		void				build_end();
		void				flush();
		void				widget_set_size(id widget_id, const VEKT_VEC2& size, helper_size_type helper_x = helper_size_type::relative, helper_size_type helper_y = helper_size_type::relative);
		void				widget_set_pos(id				  widget_id,
										   const VEKT_VEC2&	  pos,
										   helper_pos_type	  helper_x = helper_pos_type::relative,
										   helper_pos_type	  helper_y = helper_pos_type::relative,
										   helper_anchor_type anchor_x = helper_anchor_type::start,
										   helper_anchor_type anchor_y = helper_anchor_type::start);
		const VEKT_VEC2&	widget_get_size(id widget_id) const;
		const VEKT_VEC2&	widget_get_pos(id widget_id) const;
		size_props&			widget_get_size_props(id widget_id);
		pos_props&			widget_get_pos_props(id widget_id);
		VEKT_VEC4			widget_get_clip(id widget_id) const;
		widget_gfx&			widget_get_gfx(id widget);
		stroke_props&		widget_get_stroke(id widget);
		rounding_props&		widget_get_rounding(id widget);
		aa_props&			widget_get_aa(id widget);
		second_color_props& widget_get_second_color(id widget);
		text_props&			widget_get_text(id widget);
		mouse_callback&		widget_get_mouse_callbacks(id widget);
		key_callback&		widget_get_key_callbacks(id widget);
		hover_callback&		widget_get_hover_callbacks(id widget);
		void				widget_add_child(id widget_id, id child_id);
		void				widget_remove_child(id widget_id, id child_id);
		void				widget_update_text(id widget);
		void				widget_set_visible(id widget, bool is_visible);
		bool				widget_get_visible(id widget) const;

		void			   on_mouse_move(const VEKT_VEC2& mouse);
		input_event_result on_mouse_event(const mouse_event& ev);
		input_event_result on_mouse_wheel_event(const mouse_wheel_event& ev);
		input_event_result on_key_event(const key_event& ev);
		void			   add_input_layer(unsigned int priority, id root);
		void			   remove_input_layer(unsigned int priority);
		void			   add_filled_rect(const rect_props& props);
		void			   add_filled_rect_aa(const rect_props& props);
		void			   add_filled_rect_outline(const rect_props& props);
		void			   add_filled_rect_rounding(const rect_props& props);
		void			   add_filled_rect_aa_outline(const rect_props& props);
		void			   add_filled_rect_aa_rounding(const rect_props& props);
		void			   add_filled_rect_rounding_outline(const rect_props& props);
		void			   add_filled_rect_aa_outline_rounding(const rect_props& props);
		void			   add_stroke_rect(const rect_props& props);
		void			   add_stroke_rect_aa(const rect_props& props);
		void			   add_stroke_rect_rounding(const rect_props& props);
		void			   add_stroke_rect_aa_rounding(const rect_props& props);
		void			   add_text(const text_props& text, const VEKT_VEC4& color, const VEKT_VEC2& position, const VEKT_VEC2& size, unsigned int draw_order, void* user_data);
		void			   add_text_cached(const text_props& text, const VEKT_VEC4& color, const VEKT_VEC2& position, const VEKT_VEC2& size, unsigned int draw_order, void* user_data);
		static VEKT_VEC2   get_text_size(const text_props& text, const VEKT_VEC2& parent_size = VEKT_VEC2());
		draw_buffer*	   get_draw_buffer(unsigned int draw_order, void* user_data, font* fnt = nullptr);
		VEKT_VEC4		   calculate_intersection(const VEKT_VEC4& clip0, const VEKT_VEC4& clip1) const;
		id				   allocate();
		void			   deallocate(id w);
		void			   clear_text_cache();

		// Widgets
		void widget_add_debug_wrap(id widget);

		inline VEKT_VEC4 get_current_clip() const
		{
			return _clip_stack.empty() ? VEKT_VEC4() : _clip_stack[_clip_stack.size() - 1].rect;
		}

		inline void set_on_draw(draw_callback cb)
		{
			_on_draw = cb;
		}

		inline id get_root() const
		{
			return _root;
		}

	private:
		unsigned int count_total_children(id widget_id) const;
		void		 populate_hierarchy(id current_widget_id, unsigned int depth);
		void		 build_hierarchy();
		void		 calculate_sizes();
		void		 calculate_positions();
		void		 calculate_draw();
		void		 generate_rounded_rect(vector<VEKT_VEC2>& out_path, const VEKT_VEC2& min, const VEKT_VEC2& max, float rounding, int segments);
		void		 generate_sharp_rect(vector<VEKT_VEC2>& out_path, const VEKT_VEC2& min, const VEKT_VEC2& max);
		void		 generate_offset_rect_4points(vector<VEKT_VEC2>& out_path, const VEKT_VEC2& min, const VEKT_VEC2& max, float amount);
		void		 generate_offset_rect(vector<VEKT_VEC2>& out_path, const vector<VEKT_VEC2>& base_path, float amount);
		void		 add_strip(draw_buffer* db, unsigned int outer_start, unsigned int inner_start, unsigned int size, bool add_ccw);
		void		 add_filled_rect(draw_buffer* db, unsigned int start);
		void		 add_filled_rect_central(draw_buffer* db, unsigned int start, unsigned int central_start, unsigned int size);
		void		 add_vertices(draw_buffer* db, const vector<VEKT_VEC2>& path, const VEKT_VEC4& color, const VEKT_VEC2& min, const VEKT_VEC2& max);
		void		 add_vertices_multicolor(draw_buffer* db, const vector<VEKT_VEC2>& path, const VEKT_VEC4& color_start, const VEKT_VEC4& color_end, direction direction, const VEKT_VEC2& min, const VEKT_VEC2& max);
		void		 add_central_vertex(draw_buffer* db, const VEKT_VEC4& color, const VEKT_VEC2& min, const VEKT_VEC2& max);
		void		 add_central_vertex_multicolor(draw_buffer* db, const VEKT_VEC4& color_start, const VEKT_VEC4& color_end, const VEKT_VEC2& min, const VEKT_VEC2& max);
		void		 add_vertices_aa(draw_buffer* db, const vector<VEKT_VEC2>& path, unsigned int original_vertices_idx, float alpha, const VEKT_VEC2& min, const VEKT_VEC2& max);
		void		 deallocate_impl(id widget);

	private:
		struct clip_info
		{
			VEKT_VEC4	 rect  = VEKT_VEC4();
			unsigned int depth = 0;
		};

		struct depth_first_child_info
		{
			id			 widget_id		= -1;
			unsigned int depth			= 0;
			unsigned int owned_children = 0;
		};

		struct arena
		{
			void*  base_ptr = nullptr;
			size_t capacity = 0;
		};

		vector<id>	 _free_list;
		unsigned int _widget_head  = 0;
		unsigned int _widget_count = 0;

		vector<clip_info>	_clip_stack;
		vector<input_layer> _input_layers;
		vector<draw_buffer> _draw_buffers;
		id					_root			= 0;
		draw_callback		_on_draw		= nullptr;
		VEKT_VEC2			_mouse_position = {};

		arena _layout_arena = {};
		arena _gfx_arena	= {};
		arena _misc_arena	= {};

		vector<id>					   _depth_first_widgets;
		vector<id>					   _reverse_depth_first_widgets;
		vector<depth_first_child_info> _depth_first_child_info;
		vector<text_cache>			   _text_cache = {};

		widget_meta*		_metas			 = nullptr;
		size_props*			_size_properties = {};
		pos_props*			_pos_properties	 = {};
		size_result*		_size_results	 = {};
		pos_result*			_pos_results	 = {};
		widget_gfx*			_gfxs			 = {};
		stroke_props*		_strokes		 = {};
		second_color_props* _second_colors	 = {};
		rounding_props*		_roundings		 = {};
		aa_props*			_aa_props		 = {};
		text_props*			_texts			 = {};
		hover_callback*		_hover_callbacks = {};
		mouse_callback*		_mouse_callbacks = {};
		key_callback*		_key_callbacks	 = {};
		custom_passes*		_custom_passes	 = {};

		vector<VEKT_VEC2> _reuse_outer_path;
		vector<VEKT_VEC2> _reuse_inner_path;
		vector<VEKT_VEC2> _reuse_outline_path;
		vector<VEKT_VEC2> _reuse_aa_outer_path;
		vector<VEKT_VEC2> _reuse_aa_inner_path;

		vertex*		 _vertex_buffer			   = nullptr;
		index*		 _index_buffer			   = nullptr;
		vertex*		 _text_cache_vertex_buffer = nullptr;
		index*		 _text_cache_index_buffer  = nullptr;
		unsigned int _vertex_count_per_buffer  = 0;
		unsigned int _index_count_per_buffer   = 0;
		unsigned int _buffer_count			   = 0;
		unsigned int _buffer_counter		   = 0;
		unsigned int _text_cache_vertex_count  = 0;
		unsigned int _text_cache_vertex_size   = 0;
		unsigned int _text_cache_index_count   = 0;
		unsigned int _text_cache_index_size	   = 0;
	};

	////////////////////////////////////////////////////////////////////////////////
	// :: FONT & ATLAS & GLYPH
	////////////////////////////////////////////////////////////////////////////////

	class atlas;

	struct glyph
	{
		unsigned char* sdf_data			 = nullptr;
		int			   kern_advance[128] = {0};
		int			   width			 = 0;
		int			   height			 = 0;
		int			   advance_x		 = 0;
		int			   left_bearing		 = 0;
		float		   x_offset			 = 0.0f;
		float		   y_offset			 = 0.0f;
		int			   atlas_x			 = 0;
		int			   atlas_y			 = 0;
		float		   uv_x				 = 0.0f;
		float		   uv_y				 = 0.0f;
		float		   uv_w				 = 0.0f;
		float		   uv_h				 = 0.0f;
	};

	enum class font_type
	{
		normal,
		sdf,
		lcd,
	};

	struct font
	{
		glyph		 glyph_info[128];
		atlas*		 _atlas					= nullptr;
		unsigned int _atlas_required_height = 0;
		unsigned int _atlas_pos				= 0;
		float		 _scale					= 0.0f;
		int			 ascent					= 0;
		int			 descent				= 0;
		int			 line_gap				= 0;
		unsigned int size					= 0;
		font_type	 type					= font_type::normal;
		~font();
	};

	class atlas
	{
	public:
		struct slice
		{
			slice(unsigned int pos_y, unsigned int h) : pos(pos_y), height(h){};
			unsigned int pos	= 0;
			unsigned int height = 0;
		};

		atlas(unsigned int width, unsigned int height, bool is_lcd);
		~atlas();

		bool add_font(font* font);
		void remove_font(font* font);
		bool empty()
		{
			return _fonts.empty();
		}
		inline unsigned int get_width() const
		{
			return _width;
		}
		inline unsigned int get_height() const
		{
			return _height;
		}
		inline unsigned char* get_data() const
		{
			return _data;
		}
		inline unsigned int get_data_size() const
		{
			return _data_size;
		}

		inline bool get_is_lcd() const
		{
			return _is_lcd;
		}

	private:
		unsigned int   _width			 = 0;
		unsigned int   _height			 = 0;
		vector<slice*> _available_slices = {};
		vector<font*>  _fonts			 = {};
		unsigned char* _data			 = nullptr;
		unsigned int   _data_size		 = 0;
		bool		   _is_lcd			 = false;
	};

	typedef std::function<void(atlas*)> atlas_cb;

	class font_manager
	{
	public:
		font_manager(){};
		~font_manager()
		{
			ASSERT(_atlases.empty());
			ASSERT(_fonts.empty());
		};

		static inline font_manager& get()
		{
			static font_manager fm;
			return fm;
		}

		void init();
		void uninit();

		font* load_font(const char* file, unsigned int size, unsigned int range_start = 0, unsigned int range_end = 128, font_type type = font_type::normal, int sdf_padding = 3, int sdf_edge = 128, float sdf_distance = 32.0f);
		void  unload_font(font* fnt);

		inline void set_atlas_created_callback(atlas_cb cb)
		{
			_atlas_created_cb = cb;
		}
		inline void set_atlas_updated_callback(atlas_cb cb)
		{
			_atlas_updated_cb = cb;
		}
		inline void set_atlas_destroyed_callback(atlas_cb cb)
		{
			_atlas_destroyed_cb = cb;
		}

	private:
		void  find_atlas(font* fnt);
		font* load_font(const unsigned char* data, unsigned int data_size, unsigned int size, unsigned int range0, unsigned int range1, font_type type, int sdf_padding, int sdf_edge, float sdf_distance);

	private:
		vector<atlas*> _atlases;
		vector<font*>  _fonts;
		atlas_cb	   _atlas_created_cb   = nullptr;
		atlas_cb	   _atlas_updated_cb   = nullptr;
		atlas_cb	   _atlas_destroyed_cb = nullptr;
	};

}

#endif

#ifdef VEKT_IMPL

#ifndef VEKT_NO_STB_IMPL
#define STB_TRUETYPE_IMPLEMENTATION
#endif

#include "vendor/stb/stb_truetype.h"

namespace vekt
{

#define ALIGN_8(SZ) ((SZ + 7) & ~size_t(7))

	config_data config = {};

	////////////////////////////////////////////////////////////////////////////////
	// :: THEME IMPL
	////////////////////////////////////////////////////////////////////////////////

	VEKT_VEC4 theme::color_item_bg		= {3.0f / 255.0f, 3.0f / 255.0f, 3.0f / 255.0f, 1.0f};
	VEKT_VEC4 theme::color_item_hover	= {32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f, 1.0f};
	VEKT_VEC4 theme::color_item_press	= {24.0f / 255.0f, 24.0f / 255.0f, 24.0f / 255.0f, 1.0f};
	VEKT_VEC4 theme::color_panel_bg		= {24.0f / 255.0f, 24.0f / 255.0f, 24.0f / 255.0f, 1.0f};
	VEKT_VEC4 theme::color_divider		= {12.0f / 255.0f, 12.0f / 255.0f, 12.0f / 255.0f, 1.0f};
	VEKT_VEC4 theme::color_item_outline = {42.0f / 255.0f, 42.0f / 255.0f, 42.0f / 255.0f, 1.0f};
	VEKT_VEC4 theme::color_item_fg		= {200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f};
	float	  theme::item_height		= 24.0f;
	float	  theme::item_spacing		= 8.0f;
	float	  theme::indent_horizontal	= 8.0f;
	float	  theme::margin_horizontal	= 4.0f;
	float	  theme::margin_vertical	= 2.0f;
	float	  theme::border_thickness	= 6.0f;
	float	  theme::outline_thickness	= 2.0f;

	////////////////////////////////////////////////////////////////////////////////
	// :: BUILDER IMPL
	////////////////////////////////////////////////////////////////////////////////

	void builder::init(const init_config& conf)
	{
		ASSERT(conf.vertex_buffer_sz > 0 && conf.index_buffer_sz > 0 && conf.vertex_buffer_sz > 0 && conf.widget_count > 0);
		ASSERT(conf.buffer_count * sizeof(vertex) < conf.vertex_buffer_sz && conf.buffer_count * sizeof(index) < conf.index_buffer_sz);

		_widget_count = conf.widget_count;

		// Layout arena
		const size_t widget_meta_sz = ALIGN_8(sizeof(widget_meta)) * _widget_count;
		const size_t pos_props_sz	= ALIGN_8(sizeof(pos_props)) * _widget_count;
		const size_t size_props_sz	= ALIGN_8(sizeof(size_props)) * _widget_count;
		const size_t pos_result_sz	= ALIGN_8(sizeof(pos_result)) * _widget_count;
		const size_t size_result_sz = ALIGN_8(sizeof(size_result)) * _widget_count;
		_layout_arena.capacity		= widget_meta_sz + pos_props_sz + size_props_sz + pos_result_sz + size_result_sz;
		_layout_arena.base_ptr		= ALIGNED_MALLOC(_layout_arena.capacity, 8);
		MEMSET(_layout_arena.base_ptr, 0, _layout_arena.capacity);

		_metas			 = reinterpret_cast<widget_meta*>(_layout_arena.base_ptr);
		_pos_properties	 = reinterpret_cast<pos_props*>(reinterpret_cast<unsigned char*>(_layout_arena.base_ptr) + widget_meta_sz);
		_size_properties = reinterpret_cast<size_props*>(reinterpret_cast<unsigned char*>(_layout_arena.base_ptr) + widget_meta_sz + pos_props_sz);
		_pos_results	 = reinterpret_cast<pos_result*>(reinterpret_cast<unsigned char*>(_layout_arena.base_ptr) + widget_meta_sz + pos_props_sz + size_props_sz);
		_size_results	 = reinterpret_cast<size_result*>(reinterpret_cast<unsigned char*>(_layout_arena.base_ptr) + widget_meta_sz + pos_props_sz + size_props_sz + pos_result_sz);

		// Gfx arena
		const size_t widget_gfx_sz		   = ALIGN_8(sizeof(widget_gfx)) * _widget_count;
		const size_t stroke_props_sz	   = ALIGN_8(sizeof(stroke_props)) * _widget_count;
		const size_t second_color_props_sz = ALIGN_8(sizeof(second_color_props)) * _widget_count;
		const size_t rounding_props_sz	   = ALIGN_8(sizeof(rounding_props)) * _widget_count;
		const size_t aa_props_sz		   = ALIGN_8(sizeof(aa_props)) * _widget_count;
		const size_t text_props_sz		   = ALIGN_8(sizeof(text_props)) * _widget_count;
		_gfx_arena.capacity				   = widget_gfx_sz + stroke_props_sz + second_color_props_sz + rounding_props_sz + aa_props_sz + text_props_sz;
		_gfx_arena.base_ptr				   = ALIGNED_MALLOC(_gfx_arena.capacity, 8);
		MEMSET(_gfx_arena.base_ptr, 0, _gfx_arena.capacity);

		_gfxs		   = reinterpret_cast<widget_gfx*>(_gfx_arena.base_ptr);
		_strokes	   = reinterpret_cast<stroke_props*>(reinterpret_cast<unsigned char*>(_gfx_arena.base_ptr) + widget_gfx_sz);
		_second_colors = reinterpret_cast<second_color_props*>(reinterpret_cast<unsigned char*>(_gfx_arena.base_ptr) + widget_gfx_sz + stroke_props_sz);
		_roundings	   = reinterpret_cast<rounding_props*>(reinterpret_cast<unsigned char*>(_gfx_arena.base_ptr) + widget_gfx_sz + stroke_props_sz + second_color_props_sz);
		_aa_props	   = reinterpret_cast<aa_props*>(reinterpret_cast<unsigned char*>(_gfx_arena.base_ptr) + widget_gfx_sz + stroke_props_sz + second_color_props_sz + rounding_props_sz);
		_texts		   = reinterpret_cast<text_props*>(reinterpret_cast<unsigned char*>(_gfx_arena.base_ptr) + widget_gfx_sz + stroke_props_sz + second_color_props_sz + rounding_props_sz + aa_props_sz);

		// Misc-rest
		const size_t hover_callbacks_sz = ALIGN_8(sizeof(hover_callback)) * _widget_count;
		const size_t mouse_callbacks_sz = ALIGN_8(sizeof(mouse_callback)) * _widget_count;
		const size_t key_callbacks_sz	= ALIGN_8(sizeof(key_callback)) * _widget_count;
		const size_t custom_passes_sz	= ALIGN_8(sizeof(custom_passes)) * _widget_count;
		_misc_arena.capacity			= hover_callbacks_sz + mouse_callbacks_sz + key_callbacks_sz + custom_passes_sz;
		_misc_arena.base_ptr			= ALIGNED_MALLOC(_misc_arena.capacity, 8);
		MEMSET(_misc_arena.base_ptr, 0, _misc_arena.capacity);

		_hover_callbacks = reinterpret_cast<hover_callback*>(_misc_arena.base_ptr);
		_mouse_callbacks = reinterpret_cast<mouse_callback*>(reinterpret_cast<unsigned char*>(_misc_arena.base_ptr) + hover_callbacks_sz);
		_key_callbacks	 = reinterpret_cast<key_callback*>(reinterpret_cast<unsigned char*>(_misc_arena.base_ptr) + hover_callbacks_sz + mouse_callbacks_sz);
		_custom_passes	 = reinterpret_cast<custom_passes*>(reinterpret_cast<unsigned char*>(_misc_arena.base_ptr) + hover_callbacks_sz + mouse_callbacks_sz + key_callbacks_sz);

		for (size_t i = 0; i < _widget_count; i++)
		{
			new (&_metas[i]) widget_meta{};
			new (&_pos_properties[i]) pos_props{};
			new (&_size_properties[i]) size_props{};
			new (&_pos_results[i]) pos_result{};
			new (&_size_results[i]) size_result{};
			new (&_gfxs[i]) widget_gfx{};
			new (&_strokes[i]) stroke_props{};
			new (&_second_colors[i]) second_color_props{};
			new (&_roundings[i]) rounding_props{};
			new (&_aa_props[i]) aa_props{};
			new (&_texts[i]) text_props{};
			new (&_hover_callbacks[i]) hover_callback{};
			new (&_mouse_callbacks[i]) mouse_callback{};
			new (&_key_callbacks[i]) key_callback{};
			new (&_custom_passes[i]) custom_passes{};
		}

		const size_t vertex_count = conf.vertex_buffer_sz / sizeof(vertex);
		const size_t index_count  = conf.index_buffer_sz / sizeof(index);
		_vertex_count_per_buffer  = static_cast<unsigned int>(vertex_count / conf.buffer_count);
		_index_count_per_buffer	  = static_cast<unsigned int>(index_count / conf.buffer_count);
		_buffer_count			  = conf.buffer_count;

		const size_t cache_vertex_count = conf.text_cache_vertex_buffer_sz / sizeof(vertex);
		const size_t cache_index_count	= conf.text_cache_index_buffer_sz / sizeof(index);

		_vertex_buffer			  = reinterpret_cast<vertex*>(MALLOC(sizeof(vertex) * vertex_count));
		_index_buffer			  = reinterpret_cast<index*>(MALLOC(sizeof(index) * index_count));
		_text_cache_vertex_buffer = reinterpret_cast<vertex*>(MALLOC(sizeof(vertex) * cache_vertex_count));
		_text_cache_index_buffer  = reinterpret_cast<index*>(MALLOC(sizeof(index) * cache_index_count));
		_text_cache_vertex_size	  = cache_vertex_count;
		_text_cache_index_size	  = cache_index_count;

		for (size_t i = 0; i < vertex_count; i++)
			new (&_vertex_buffer[i]) vertex();
		for (size_t i = 0; i < index_count; i++)
			new (&_index_buffer[i]) index();

		const size_t total_sz = _layout_arena.capacity + _gfx_arena.capacity + _misc_arena.capacity + conf.vertex_buffer_sz + conf.index_buffer_sz + conf.text_cache_vertex_buffer_sz + conf.text_cache_index_buffer_sz;
		V_LOG("Vekt builder initialized with %d widgets. Total memory reserved: %zu bytes - %0.2f mb", _widget_count, total_sz, static_cast<float>(total_sz) / 1000000.f);

		_root = allocate();
	}

	void builder::uninit()
	{
		deallocate(_root);

		for (size_t i = 0; i < _widget_count; i++)
		{
			_metas[i].~widget_meta();
			_pos_properties[i].~pos_props();
			_size_properties[i].~size_props();
			_pos_results[i].~pos_result();
			_size_results[i].~size_result();
			_gfxs[i].~widget_gfx();
			_strokes[i].~stroke_props();
			_second_colors[i].~second_color_props();
			_roundings[i].~rounding_props();
			_aa_props[i].~aa_props();
			_texts[i].~text_props();
			_hover_callbacks[i].~hover_callback();
			_mouse_callbacks[i].~mouse_callback();
			_key_callbacks[i].~key_callback();
			_custom_passes[i].~custom_passes();
		}

		ALIGNED_FREE(_layout_arena.base_ptr);
		ALIGNED_FREE(_gfx_arena.base_ptr);
		ALIGNED_FREE(_misc_arena.base_ptr);
		_layout_arena.base_ptr = nullptr;
		_gfx_arena.base_ptr	   = nullptr;
		_misc_arena.base_ptr   = nullptr;

		if (_vertex_buffer)
			FREE(_vertex_buffer);
		if (_index_buffer)
			FREE(_index_buffer);
		if (_text_cache_vertex_buffer)
			FREE(_text_cache_vertex_buffer);
		if (_text_cache_index_buffer)
			FREE(_text_cache_index_buffer);

		_vertex_buffer = nullptr;
		_index_buffer  = nullptr;
	}

	void builder::build_begin(const VEKT_VEC2& screen_size)
	{
		_draw_buffers.resize(0);
		_clip_stack.resize_explicit(0);
		_buffer_counter = 0;

		/* size & pos & draw */
		builder& bd = *this;

		pos_props& root_pos = _pos_properties[_root];
		root_pos.pos		= VEKT_VEC2();
		root_pos.flags |= pos_flags::pf_x_abs | pos_flags::pf_y_abs;

		size_props& root_size = _size_properties[_root];
		root_size.size		  = screen_size;
		root_size.flags |= size_flags::sf_x_abs | size_flags::sf_y_abs;

		calculate_sizes();
		calculate_positions();

		_clip_stack.resize_explicit(0);
		_clip_stack.push_back({{0.0f, 0.0f, screen_size.x, screen_size.y}, 0});
		calculate_draw();
	}

	void builder::build_end()
	{
		_clip_stack.pop_back();
	}

	void builder::flush()
	{
		if (!_on_draw)
			return;

		std::sort(_draw_buffers.begin(), _draw_buffers.end(), [](const draw_buffer& a, const draw_buffer& b) { return a.draw_order < b.draw_order; });

		for (draw_buffer& db : _draw_buffers)
			_on_draw(db);
	}

	void builder::widget_add_child(id widget_id, id child_id)
	{
		widget_meta& meta		= _metas[widget_id];
		widget_meta& child_meta = _metas[child_id];

		child_meta.parent = widget_id;
		meta.children.push_back(child_id);

		build_hierarchy();
	}

	void builder::widget_remove_child(id widget_id, id child_id)
	{
		widget_meta& meta		= _metas[widget_id];
		widget_meta& child_meta = _metas[child_id];

		meta.children.remove(child_id);
		child_meta.parent = -1;

		build_hierarchy();
	}

	inline VEKT_VEC4 builder::widget_get_clip(id widget_id) const
	{
		const size_result& sz = _size_results[widget_id];
		const pos_result&  ps = _pos_results[widget_id];
		return {ps.pos.x, ps.pos.y, sz.size.x, sz.size.y};
	}

	widget_gfx& builder::widget_get_gfx(id widget)
	{
		return _gfxs[widget];
	}
	stroke_props& builder::widget_get_stroke(id widget)
	{
		return _strokes[widget];
	}
	rounding_props& builder::widget_get_rounding(id widget)
	{
		return _roundings[widget];
	}
	aa_props& builder::widget_get_aa(id widget)
	{
		return _aa_props[widget];
	}
	second_color_props& builder::widget_get_second_color(id widget)
	{
		return _second_colors[widget];
	}
	text_props& builder::widget_get_text(id widget)
	{
		return _texts[widget];
	}

	mouse_callback& builder::widget_get_mouse_callbacks(id widget)
	{
		return _mouse_callbacks[widget];
	}

	key_callback& builder::widget_get_key_callbacks(id widget)
	{
		return _key_callbacks[widget];
	}

	hover_callback& builder::widget_get_hover_callbacks(id widget)
	{
		return _hover_callbacks[widget];
	}

	void builder::widget_update_text(id widget)
	{
		widget_gfx& gfx = _gfxs[widget];
		gfx.flags		= gfx_is_text;

		size_props& sz = _size_properties[widget];
		sz.flags	   = sf_x_abs | sf_y_abs;

		text_props& props = _texts[widget];
		sz.size			  = get_text_size(props);
	}

	void builder::widget_set_visible(id widget, bool is_visible)
	{
		if (is_visible)
			_gfxs[widget].flags &= ~gfx_invisible;
		else
			_gfxs[widget].flags |= gfx_invisible;
	}

	bool builder::widget_get_visible(id widget) const
	{
		return _gfxs[widget].flags & gfx_invisible;
	}

	id builder::allocate()
	{
		if (!_free_list.empty())
		{
			const id w = _free_list.get_back();
			_free_list.pop_back();
			return w;
		}

		const id idx = static_cast<id>(_widget_head);
		_widget_head++;
		ASSERT(_widget_head < _widget_count);
		return idx;
	}

	void builder::deallocate_impl(id w)
	{
		widget_meta& meta = _metas[w];

		for (id c : meta.children)
			deallocate_impl(c);

		_metas[w]			= widget_meta{};
		_size_properties[w] = size_props{};
		_pos_properties[w]	= pos_props{};
		_size_results[w]	= size_result{};
		_pos_results[w]		= pos_result{};
		_gfxs[w]			= widget_gfx{};
		_strokes[w]			= stroke_props{};
		_second_colors[w]	= second_color_props{};
		_roundings[w]		= rounding_props{};
		_aa_props[w]		= aa_props{};
		_texts[w]			= text_props{};
		_hover_callbacks[w] = hover_callback{};
		_mouse_callbacks[w] = mouse_callback{};
		_key_callbacks[w]	= key_callback{};
		_custom_passes[w]	= custom_passes{};

		_free_list.push_back(w);
	}

	void builder::deallocate(id w)
	{
		for (const input_layer& layer : _input_layers)
		{
			if (layer.root == w)
			{
				V_ERR("Cannot deallocate widget %d, it is a root of an input layer. Remove the input layer first!", w);
				ASSERT(false);
				return;
			}
		}

		widget_meta& meta = _metas[w];

		if (meta.parent != -1)
		{
			widget_meta& parent_meta = _metas[meta.parent];
			parent_meta.children.remove(w);
		}

		deallocate_impl(w);

		build_hierarchy();
	}

	void builder::clear_text_cache()
	{
		_text_cache.resize_explicit(0);
	}

	unsigned int builder::count_total_children(id widget_id) const
	{
		unsigned int	   count = 0;
		const widget_meta& meta	 = _metas[widget_id];
		for (id child : meta.children)
		{
			count++;
			count += count_total_children(child);
		}
		return count;
	}

	void builder::populate_hierarchy(id current_widget_id, unsigned int depth)
	{
		_depth_first_widgets.push_back(current_widget_id);
		_depth_first_child_info.push_back({current_widget_id, depth, count_total_children(current_widget_id)});

		widget_meta&	   current	   = _metas[current_widget_id];
		const unsigned int child_depth = ++depth;
		for (id child : current.children)
			populate_hierarchy(child, child_depth);
	}

	void builder::build_hierarchy()
	{
		_depth_first_widgets.resize_explicit(0);
		_depth_first_child_info.resize_explicit(0);
		populate_hierarchy(_root, 0);
		_reverse_depth_first_widgets = _depth_first_widgets;
		_reverse_depth_first_widgets.reverse();
	}

	void builder::calculate_sizes()
	{

		// top-down
		for (id widget : _depth_first_widgets)
		{
			const size_props& sz = _size_properties[widget];

			if (sz.flags & size_flags::sf_custom_pass)
			{
				custom_passes& passes = _custom_passes[widget];
				if (passes.custom_size_pass)
					passes.custom_size_pass(this, widget);
				continue;
			}

			const widget_meta& meta = _metas[widget];

			VEKT_VEC2 final_size = sz.size;

			if (sz.flags & size_flags::sf_x_abs)
				final_size.x = sz.size.x;
			if (sz.flags & size_flags::sf_y_abs)
				final_size.y = sz.size.y;

			const bool x_relative = sz.flags & size_flags::sf_x_relative;
			const bool y_relative = sz.flags & size_flags::sf_y_relative;

			if (x_relative || y_relative)
			{
				const size_props&  parent_sz  = _size_properties[meta.parent];
				const size_result& parent_res = _size_results[meta.parent];
				if (x_relative)
					final_size.x = (parent_res.size.x - parent_sz.child_margins.left - parent_sz.child_margins.right) * sz.size.x;
				if (y_relative)
					final_size.y = (parent_res.size.y - parent_sz.child_margins.top - parent_sz.child_margins.bottom) * sz.size.y;
			}

			if (sz.flags & size_flags::sf_x_copy_y)
				final_size.x = final_size.y;
			else if (sz.flags & size_flags::sf_y_copy_x)
				final_size.y = final_size.x;

			size_result& res = _size_results[widget];
			res.size		 = final_size;
		}

		// bottom-up
		vector<id> fill_x_children;
		vector<id> fill_y_children;
		id		   fill_parent = -1;
		for (id widget : _reverse_depth_first_widgets)
		{
			const size_props&  sz	= _size_properties[widget];
			const widget_meta& meta = _metas[widget];

			VEKT_VEC2 final_size = VEKT_VEC2();

			if (sz.flags & size_flags::sf_x_max_children || sz.flags & size_flags::sf_y_max_children || sz.flags & size_flags::sf_x_total_children || sz.flags & size_flags::sf_y_total_children)
			{
				for (id child : meta.children)
				{
					const size_result& child_res = _size_results[child];

					if (sz.flags & size_flags::sf_x_max_children)
						final_size.x = math::max(child_res.size.x, final_size.x);
					else
						final_size.x += child_res.size.x + sz.spacing;

					if (sz.flags & size_flags::sf_y_max_children)
						final_size.y = math::max(child_res.size.y + sz.child_margins.top + sz.child_margins.bottom, final_size.y);
					else
						final_size.y += child_res.size.y + sz.spacing;
				}

				if (sz.flags & size_flags::sf_x_total_children && !math::equals(final_size.x, 0.0f, 0.001f))
					final_size.x -= sz.spacing;
				if (sz.flags & size_flags::sf_x_total_children && !math::equals(final_size.y, 0.0f, 0.001f))
					final_size.y -= sz.spacing;

				final_size.x += sz.child_margins.left + sz.child_margins.right;
				final_size.y += sz.child_margins.top + sz.child_margins.bottom;

				if (sz.flags & size_flags::sf_x_copy_y)
					final_size.x = final_size.y;
				else if (sz.flags & size_flags::sf_y_copy_x)
					final_size.y = final_size.x;

				size_result& res = _size_results[widget];
				res.size		 = final_size;
			}

			if (widget == fill_parent)
			{
				fill_parent			= -1;
				size_result& res	= _size_results[widget];
				float		 x_left = res.size.x - sz.child_margins.left - sz.child_margins.right;
				float		 y_left = res.size.y - sz.child_margins.top - sz.child_margins.bottom;

				for (id child : meta.children)
				{
					const size_result& child_res = _size_results[child];

					if (fill_x_children.find(child) == fill_x_children.end())
						x_left -= child_res.size.x;
					if (fill_y_children.find(child) == fill_y_children.end())
						y_left -= child_res.size.y;
				}

				for (id child : fill_x_children)
				{
					size_result&	  child_res	  = _size_results[child];
					const size_props& child_props = _size_properties[child];

					child_res.size.x = x_left / static_cast<float>(fill_x_children.size());

					if (child_props.flags & size_flags::sf_x_copy_y)
						child_res.size.y = child_res.size.x;
				}

				for (id child : fill_y_children)
				{
					size_result&	  child_res	  = _size_results[child];
					const size_props& child_props = _size_properties[child];
					child_res.size.y			  = y_left / static_cast<float>(fill_y_children.size());

					if (child_props.flags & size_flags::sf_y_copy_x)
						child_res.size.x = child_res.size.y;
				}

				fill_y_children.resize(0);
				fill_x_children.resize(0);
			}

			if (sz.flags & size_flags::sf_x_fill)
			{
				fill_x_children.push_back(widget);
				fill_parent = meta.parent;
			}

			if (sz.flags & size_flags::sf_y_fill)
			{
				fill_y_children.push_back(widget);
				fill_parent = meta.parent;
			}
		}
	}

	void builder::calculate_positions()
	{
		for (id widget : _depth_first_widgets)
		{
			pos_props& pp = _pos_properties[widget];

			if (pp.flags & pos_flags::pf_custom_pass)
			{
				custom_passes& passes = _custom_passes[widget];
				if (passes.custom_pos_pass)
					passes.custom_pos_pass(this, widget);
				continue;
			}

			size_result& sr = _size_results[widget];
			pos_result&	 pr = _pos_results[widget];

			VEKT_VEC2 final_pos = pr.pos;

			if (pp.flags & pos_flags::pf_x_abs)
				final_pos.x = pp.pos.x;
			if (pp.flags & pos_flags::pf_y_abs)
				final_pos.y = pp.pos.y;

			const bool x_relative = pp.flags & pos_flags::pf_x_relative;
			const bool y_relative = pp.flags & pos_flags::pf_y_relative;

			if (x_relative || y_relative)
			{
				widget_meta& meta			  = _metas[widget];
				pos_result&	 parent_result	  = _pos_results[meta.parent];
				size_result& parent_sz_result = _size_results[meta.parent];
				size_props&	 parent_sz_props  = _size_properties[meta.parent];

				if (x_relative)
				{
					const float parent_width = parent_sz_result.size.x;

					if (pp.flags & pos_flags::pf_x_anchor_end)
						final_pos.x = (parent_result.pos.x + parent_sz_props.child_margins.left) + (parent_width * pp.pos.x) - sr.size.x;
					else if (pp.flags & pos_flags::pf_x_anchor_center)
						final_pos.x = (parent_result.pos.x + parent_sz_props.child_margins.left) + (parent_width * pp.pos.x) - sr.size.x * 0.5f;
					else
						final_pos.x = (parent_result.pos.x + parent_sz_props.child_margins.left) + (parent_width * pp.pos.x);
				}

				if (y_relative)
				{
					const float parent_height = parent_sz_result.size.y;

					if (pp.flags & pos_flags::pf_y_anchor_end)
						final_pos.y = (parent_result.pos.y + parent_sz_props.child_margins.top) + (parent_height * pp.pos.y) - sr.size.x;
					else if (pp.flags & pos_flags::pf_y_anchor_center)
						final_pos.y = (parent_result.pos.y + parent_sz_props.child_margins.top) + (parent_height * pp.pos.y) - sr.size.y * 0.5f;
					else
						final_pos.y = (parent_result.pos.y + parent_sz_props.child_margins.top) + (parent_height * pp.pos.y);
				}
			}

			pr.pos = final_pos;
		}

		for (id widget : _depth_first_widgets)
		{
			pos_props&	 pp		   = _pos_properties[widget];
			widget_meta& meta	   = _metas[widget];
			VEKT_VEC2	 final_pos = _pos_results[widget].pos;

			if (pp.flags & pf_child_pos_row)
			{
				size_props& sp = _size_properties[widget];

				float child_x = final_pos.x + sp.child_margins.left;

				for (id child : meta.children)
				{
					pos_result&	 child_res		= _pos_results[child];
					size_result& child_size_res = _size_results[child];
					child_res.pos.x				= child_x;
					child_x += sp.spacing + child_size_res.size.x;
				}
			}
			else if (pp.flags & pf_child_pos_column)
			{
				size_props& sp = _size_properties[widget];

				float child_y = final_pos.y + sp.child_margins.top + pp.scroll_offset;

				for (id child : meta.children)
				{
					pos_result&	 child_res		= _pos_results[child];
					size_result& child_size_res = _size_results[child];
					child_res.pos.y				= child_y;
					child_y += sp.spacing + child_size_res.size.y;
				}
			}
		}
	}

	void builder::calculate_draw()
	{
		VEKT_VEC4 second_color;
		direction color_direction = direction::horizontal;
		bool	  multi_color	  = false;

		const unsigned int sz = _depth_first_child_info.size();

		for (unsigned int i = 1; i < sz;)
		{
			const depth_first_child_info& info	 = _depth_first_child_info[i];
			const id					  widget = info.widget_id;

			// left-over clip stacks from previous widgets, always pop until we reach the depth of the current widget.
			while (_clip_stack.size() > 0 && _clip_stack.get_back().depth >= info.depth)
				_clip_stack.pop_back();

			/*
				Both invisible widgets and clipped widgets skip the dfo list by total children amount.
			*/
			widget_gfx& gfx = _gfxs[widget];

			if (gfx.flags & gfx_invisible)
			{
				i += info.owned_children + 1;
				continue;
			}

			const VEKT_VEC2& pos		  = _pos_results[widget].pos;
			const VEKT_VEC2& size		  = _size_results[widget].size;
			const VEKT_VEC4	 widget_clip  = VEKT_VEC4(pos.x, pos.y, size.x, size.y);
			const VEKT_VEC4	 intersection = calculate_intersection(_clip_stack.get_back().rect, widget_clip);
			const bool		 has_clip	  = gfx.flags & gfx_flags::gfx_clip_children;
			if (intersection.z <= 0 || intersection.w <= 0)
			{
				i += info.owned_children + 1;
				continue;
			}

			if (gfx.flags & gfx_flags::gfx_custom_pass)
			{
				custom_passes& passes = _custom_passes[widget];
				if (passes.custom_draw_pass)
					passes.custom_draw_pass(this, widget);
				if (has_clip)
					_clip_stack.push_back({widget_clip, info.depth});
				i++;
				continue;
			}

			multi_color = false;
			if (gfx.flags & gfx_has_second_color)
			{
				second_color_props& p = widget_get_second_color(widget);
				second_color		  = p.color;
				color_direction		  = p.direction;
				multi_color			  = true;
			}

			const rect_props props = {
				.gfx			 = gfx,
				.min			 = pos,
				.max			 = pos + size,
				.use_hovered	 = false,
				.use_pressed	 = false,
				.color_start	 = gfx.color,
				.color_end		 = second_color,
				.color_direction = color_direction,
				.widget_id		 = widget,
				.multi_color	 = multi_color,
			};

			const bool has_aa		= gfx.flags & gfx_has_aa;
			const bool has_outline	= gfx.flags & gfx_has_stroke;
			const bool has_rounding = gfx.flags & gfx_has_rounding;

			if (gfx.flags & gfx_is_rect)
			{
				if (has_aa && has_outline && has_rounding)
					add_filled_rect_aa_outline_rounding(props);
				else if (has_aa && has_outline && !has_rounding)
					add_filled_rect_aa_outline(props);
				else if (has_aa && !has_outline && !has_rounding)
					add_filled_rect_aa(props);
				else if (has_aa && !has_outline && has_rounding)
					add_filled_rect_aa_rounding(props);
				else if (has_outline && !has_aa && !has_rounding)
					add_filled_rect_outline(props);
				else if (has_outline && has_rounding && !has_aa)
					add_filled_rect_rounding_outline(props);
				else if (has_rounding && !has_aa && !has_outline)
					add_filled_rect_rounding(props);
				else
					add_filled_rect(props);
			}
			else if (gfx.flags & gfx_is_stroke)
			{
				if (has_aa && has_rounding)
					add_stroke_rect_aa_rounding(props);
				else if (has_aa && !has_rounding)
					add_stroke_rect_aa(props);
				else if (!has_aa && has_rounding)
					add_stroke_rect_rounding(props);
				else
					add_stroke_rect(props);
			}
			else if (gfx.flags & gfx_is_text)
			{
				add_text(_texts[widget], gfx.color, pos, size, gfx.draw_order, gfx.user_data);
			}
			else if (gfx.flags & gfx_is_text_cached)
			{
				add_text_cached(_texts[widget], gfx.color, pos, size, gfx.draw_order, gfx.user_data);
			}

			if (has_clip)
				_clip_stack.push_back({widget_clip, info.depth});

			i++;
		}
	}

	void builder::widget_set_size(id widget_id, const VEKT_VEC2& size, helper_size_type helper_x, helper_size_type helper_y)
	{
		size_props& props = _size_properties[widget_id];
		props.size		  = size;
		props.flags		  = 0;

		switch (helper_x)
		{
		case helper_size_type::absolute:
			props.flags |= size_flags::sf_x_abs;
			break;
		case helper_size_type::relative:
			props.flags |= size_flags::sf_x_relative;
			break;
		case helper_size_type::fill:
			props.flags |= size_flags::sf_x_fill;
			break;
		case helper_size_type::max_children:
			props.flags |= size_flags::sf_x_max_children;
			break;
		case helper_size_type::total_children:
			props.flags |= size_flags::sf_x_total_children;
			break;
		case helper_size_type::copy_other:
			props.flags |= size_flags::sf_x_copy_y;
			break;
		default:
			break;
		}

		switch (helper_y)
		{
		case helper_size_type::absolute:
			props.flags |= size_flags::sf_y_abs;
			break;
		case helper_size_type::relative:
			props.flags |= size_flags::sf_y_relative;
			break;
		case helper_size_type::fill:
			props.flags |= size_flags::sf_y_fill;
			break;
		case helper_size_type::max_children:
			props.flags |= size_flags::sf_y_max_children;
			break;
		case helper_size_type::total_children:
			props.flags |= size_flags::sf_y_total_children;
			break;
		case helper_size_type::copy_other:
			props.flags |= size_flags::sf_y_copy_x;
			break;
		default:
			break;
		}
	}

	inline void builder::widget_set_pos(id widget_id, const VEKT_VEC2& pos, helper_pos_type helper_x, helper_pos_type helper_y, helper_anchor_type anchor_x, helper_anchor_type anchor_y)
	{
		pos_props& props = _pos_properties[widget_id];
		props.pos		 = pos;
		props.flags &= ~pos_flags::pf_x_relative;
		props.flags &= ~pos_flags::pf_y_relative;
		props.flags &= ~pos_flags::pf_x_abs;
		props.flags &= ~pos_flags::pf_y_abs;
		props.flags &= ~pos_flags::pf_x_anchor_center;
		props.flags &= ~pos_flags::pf_y_anchor_center;
		props.flags &= ~pos_flags::pf_x_anchor_end;
		props.flags &= ~pos_flags::pf_y_anchor_end;

		switch (helper_x)
		{
		case helper_pos_type::relative:
			props.flags |= pos_flags::pf_x_relative;
			break;
		case helper_pos_type::absolute:
			props.flags |= pos_flags::pf_x_abs;
			break;
		default:
			break;
		}

		switch (anchor_x)
		{
		case helper_anchor_type::center:
			props.flags |= pos_flags::pf_x_anchor_center;
			break;
		case helper_anchor_type::end:
			props.flags |= pos_flags::pf_x_anchor_end;
			break;
		default:
			break;
		}

		switch (helper_y)
		{
		case helper_pos_type::relative:
			props.flags |= pos_flags::pf_y_relative;
			break;
		case helper_pos_type::absolute:
			props.flags |= pos_flags::pf_y_abs;
			break;
		default:
			break;
		}

		switch (anchor_y)
		{
		case helper_anchor_type::center:
			props.flags |= pos_flags::pf_y_anchor_center;
			break;
		case helper_anchor_type::end:
			props.flags |= pos_flags::pf_y_anchor_end;
			break;
		default:
			break;
		}
	}

	const VEKT_VEC2& builder::widget_get_size(id widget_id) const
	{
		return _size_results[widget_id].size;
	}

	const VEKT_VEC2& builder::widget_get_pos(id widget_id) const
	{
		return _pos_results[widget_id].pos;
	}

	size_props& builder::widget_get_size_props(id widget_id)
	{
		return _size_properties[widget_id];
	}

	inline pos_props& builder::widget_get_pos_props(id widget_id)
	{
		return _pos_properties[widget_id];
	}

	void builder::on_mouse_move(const VEKT_VEC2& mouse)
	{
		const VEKT_VEC2 delta = mouse - _mouse_position;
		_mouse_position		  = mouse;

		const unsigned int sz = _depth_first_widgets.size();
		for (unsigned int i = 0; i < sz; i++)
		{
			const id widget = _depth_first_widgets[i];

			const VEKT_VEC4 clip	= widget_get_clip(widget);
			const bool		hovered = clip.is_point_inside(mouse.x, mouse.y);

			hover_callback& hover_state = _hover_callbacks[widget];

			if (!hovered && hover_state.is_hovered && hover_state.on_hover_end)
				hover_state.on_hover_end(this, widget);
			if (hovered && !hover_state.is_hovered && hover_state.on_hover_begin)
				hover_state.on_hover_begin(this, widget);
			hover_state.is_hovered = hovered;
		}
	}

	input_event_result builder::on_mouse_event(const mouse_event& ev)
	{
		if (_input_layers.empty())
		{
			V_ERR("vekt::builder::on_mouse_event -> No input layers are added to the builder!");
			return input_event_result::not_handled;
		}

		for (const input_layer& layer : _input_layers)
		{
			depth_first_child_info& root_info = _depth_first_child_info[layer.root];
			input_event_result		res		  = input_event_result::not_handled;
			const int				root_idx  = _depth_first_widgets.index_of(layer.root);

			int begin = root_idx;
			int end	  = root_idx + static_cast<int>(root_info.owned_children) + 1;

			for (int i = begin; i < end; i++)
			{
				const id		widget = _depth_first_widgets[i];
				mouse_callback& ms	   = _mouse_callbacks[widget];

				if (ms.on_mouse)
				{
					res = ms.on_mouse(this, widget, ev, input_event_phase::tunneling);
					if (res == input_event_result::handled)
						break;
				}
			}
			if (res == input_event_result::handled)
			{
				return res;
			}

			begin = end - 1;
			end	  = root_idx;

			for (int i = begin; i >= end; i--)
			{
				const id		widget = _depth_first_widgets[i];
				mouse_callback& ms	   = _mouse_callbacks[widget];
				if (ms.on_mouse)
				{
					res = ms.on_mouse(this, widget, ev, input_event_phase::bubbling);
					if (res == input_event_result::handled)
						break;
				}
			}

			if (res == input_event_result::handled)
			{
				return res;
			}
		}

		return input_event_result::not_handled;
	}

	input_event_result builder::on_mouse_wheel_event(const mouse_wheel_event& ev)
	{
		if (_input_layers.empty())
		{
			V_ERR("vekt::builder::on_mouse_wheel_event -> No input layers are added to the builder!");
			return input_event_result::not_handled;
		}

		for (const input_layer& layer : _input_layers)
		{
			depth_first_child_info& root_info = _depth_first_child_info[layer.root];
			input_event_result		res		  = input_event_result::not_handled;
			const int				root_idx  = _depth_first_widgets.index_of(layer.root);

			int begin = root_idx;
			int end	  = root_idx + static_cast<int>(root_info.owned_children) + 1;

			for (int i = begin; i < end; i++)
			{
				const id		widget = _depth_first_widgets[i];
				mouse_callback& ms	   = _mouse_callbacks[widget];

				if (ms.on_mouse_wheel)
				{
					res = ms.on_mouse_wheel(this, widget, ev);
					if (res == input_event_result::handled)
						return res;
				}
			}
		}

		return input_event_result::not_handled;
	}

	input_event_result builder::on_key_event(const key_event& ev)
	{
		if (_input_layers.empty())
		{
			V_ERR("vekt::builder::on_key_event -> No input layers are added to the builder!");
			return input_event_result::not_handled;
		}

		for (const input_layer& layer : _input_layers)
		{
			depth_first_child_info& root_info = _depth_first_child_info[layer.root];
			input_event_result		res		  = input_event_result::not_handled;
			const int				root_idx  = _depth_first_widgets.index_of(layer.root);

			int begin = root_idx;
			int end	  = root_idx + static_cast<int>(root_info.owned_children) + 1;

			for (int i = begin; i < end; i++)
			{
				const id	  widget = _depth_first_widgets[i];
				key_callback& ks	 = _key_callbacks[widget];

				if (ks.on_key)
				{
					res = ks.on_key(this, widget, ev);
					if (res == input_event_result::handled)
						return res;
				}
			}
		}

		return input_event_result::not_handled;
	}

	void builder::add_input_layer(unsigned int priority, id root)
	{
		for (input_layer& layer : _input_layers)
		{
			if (layer.priority == priority)
			{
				V_WARN("vekt::builder::add_input_layer -> Input layer with this priority already exists, overriding it's root! priority: %d", priority);
				return;
			}
		}

		_input_layers.push_back({
			.priority = priority,
			.root	  = root,
		});

		std::sort(_input_layers.begin(), _input_layers.end(), [](const input_layer& layer0, const input_layer& layer1) -> bool { return layer0.priority < layer1.priority; });
	}

	void builder::remove_input_layer(unsigned int priority)
	{
		for (input_layer& layer : _input_layers)
		{
			if (layer.priority == priority)
			{
				_input_layers.remove(layer);
				std::sort(_input_layers.begin(), _input_layers.end(), [](const input_layer& layer0, const input_layer& layer1) -> bool { return layer0.priority < layer1.priority; });
				return;
			}
		}
		V_ERR("vekt::remove_input_layer -> No input layer with the given priority exists! priority: %d", priority);
	}

	void builder::add_filled_rect(const rect_props& props)
	{
		draw_buffer* db = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);
		_reuse_outer_path.resize_explicit(0);
		generate_sharp_rect(_reuse_outer_path, props.min, props.max);

		const unsigned int out_start = db->vertex_count;

		if (props.multi_color)
			add_vertices_multicolor(db, _reuse_outer_path, props.color_start, props.color_end, props.color_direction, props.min, props.max);
		else
			add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);

		add_filled_rect(db, out_start);
	}

	void builder::add_filled_rect_aa(const rect_props& props)
	{
		_reuse_outer_path.resize_explicit(0);
		_reuse_aa_outer_path.resize_explicit(0);

		aa_props& p = _aa_props[props.widget_id];

		generate_sharp_rect(_reuse_outer_path, props.min, props.max);
		generate_offset_rect(_reuse_aa_outer_path, _reuse_outer_path, -static_cast<float>(p.thickness));

		draw_buffer*	   db		 = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);
		const unsigned int out_start = db->vertex_count;

		if (props.multi_color)
			add_vertices_multicolor(db, _reuse_outer_path, props.color_start, props.color_end, props.color_direction, props.min, props.max);
		else
			add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);

		add_filled_rect(db, out_start);

		const unsigned int out_aa_start = db->vertex_count;
		add_vertices_aa(db, _reuse_aa_outer_path, out_start, 0.0f, props.min, props.max);
		add_strip(db, out_aa_start, out_start, _reuse_aa_outer_path.size(), false);
	}

	inline void builder::add_filled_rect_outline(const rect_props& props)
	{
		draw_buffer* db = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);

		stroke_props& out_p = _strokes[props.widget_id];

		_reuse_outer_path.resize_explicit(0);
		_reuse_outline_path.resize_explicit(0);
		_reuse_aa_outer_path.resize_explicit(0);

		generate_sharp_rect(_reuse_outer_path, props.min, props.max);

		generate_offset_rect(_reuse_outline_path, _reuse_outer_path, -static_cast<float>(out_p.thickness));

		const unsigned int out_start = db->vertex_count;

		if (props.multi_color)
			add_vertices_multicolor(db, _reuse_outer_path, props.color_start, props.color_end, props.color_direction, props.min, props.max);
		else
			add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);

		add_filled_rect(db, out_start);

		unsigned int outline_start = 0;
		// add original vertices
		const unsigned int copy_start = db->vertex_count;
		add_vertices(db, _reuse_outer_path, out_p.color, props.min, props.max);

		outline_start = db->vertex_count;
		add_vertices(db, _reuse_outline_path, out_p.color, props.min, props.max);
		add_strip(db, outline_start, copy_start, _reuse_outline_path.size(), false);
	}

	inline void builder::add_filled_rect_rounding(const rect_props& props)
	{
		draw_buffer* db = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);

		rounding_props& rp = _roundings[props.widget_id];

		_reuse_outer_path.resize_explicit(0);
		_reuse_outline_path.resize_explicit(0);
		_reuse_aa_outer_path.resize_explicit(0);

		const bool has_rounding = rp.rounding > 0.0f;
		generate_rounded_rect(_reuse_outer_path, props.min, props.max, rp.rounding, rp.segments);

		const unsigned int out_start	 = db->vertex_count;
		const unsigned int central_start = out_start + _reuse_outer_path.size();

		if (props.multi_color)
		{
			add_vertices_multicolor(db, _reuse_outer_path, props.color_start, props.color_end, props.color_direction, props.min, props.max);
			add_central_vertex_multicolor(db, props.color_start, props.color_end, props.min, props.max);
		}
		else
		{
			add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
			add_central_vertex(db, props.color_start, props.min, props.max);
		}

		add_filled_rect_central(db, out_start, central_start, _reuse_outer_path.size());
	}

	inline void builder::add_filled_rect_aa_outline(const rect_props& props)
	{
		draw_buffer* db = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);

		aa_props&	  aa_p	= _aa_props[props.widget_id];
		stroke_props& out_p = _strokes[props.widget_id];

		_reuse_outer_path.resize_explicit(0);
		_reuse_outline_path.resize_explicit(0);
		_reuse_aa_outer_path.resize_explicit(0);

		generate_sharp_rect(_reuse_outer_path, props.min, props.max);
		generate_offset_rect(_reuse_outline_path, _reuse_outer_path, -static_cast<float>(out_p.thickness));

		generate_offset_rect(_reuse_aa_outer_path, _reuse_outline_path, -static_cast<float>(aa_p.thickness));

		const unsigned int out_start = db->vertex_count;

		if (props.multi_color)
			add_vertices_multicolor(db, _reuse_outer_path, props.color_start, props.color_end, props.color_direction, props.min, props.max);
		else
			add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
		add_filled_rect(db, out_start);

		unsigned int outline_start = 0;

		// add original vertices
		const unsigned int copy_start = db->vertex_count;
		add_vertices(db, _reuse_outer_path, out_p.color, props.min, props.max);

		outline_start = db->vertex_count;
		add_vertices(db, _reuse_outline_path, out_p.color, props.min, props.max);
		add_strip(db, outline_start, copy_start, _reuse_outline_path.size(), false);

		const unsigned int out_aa_start = db->vertex_count;
		add_vertices_aa(db, _reuse_aa_outer_path, outline_start, 0.0f, props.min, props.max);
		add_strip(db, out_aa_start, outline_start, _reuse_aa_outer_path.size(), false);
	}

	inline void builder::add_filled_rect_aa_rounding(const rect_props& props)
	{
		draw_buffer*	db	 = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);
		aa_props&		aa_p = _aa_props[props.widget_id];
		rounding_props& rp	 = _roundings[props.widget_id];

		_reuse_outer_path.resize_explicit(0);
		_reuse_outline_path.resize_explicit(0);
		_reuse_aa_outer_path.resize_explicit(0);

		generate_rounded_rect(_reuse_outer_path, props.min, props.max, rp.rounding, rp.segments);

		generate_offset_rect(_reuse_aa_outer_path, _reuse_outer_path, -static_cast<float>(aa_p.thickness));

		const unsigned int out_start	 = db->vertex_count;
		const unsigned int central_start = out_start + _reuse_outer_path.size();

		if (props.multi_color)
		{
			add_vertices_multicolor(db, _reuse_outer_path, props.color_start, props.color_end, props.color_direction, props.min, props.max);
			add_central_vertex_multicolor(db, props.color_start, props.color_end, props.min, props.max);
		}
		else
		{
			add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
			add_central_vertex(db, props.color_start, props.min, props.max);
		}

		add_filled_rect_central(db, out_start, central_start, _reuse_outer_path.size());

		unsigned int outline_start = 0;

		const unsigned int out_aa_start = db->vertex_count;
		add_vertices_aa(db, _reuse_aa_outer_path, out_start, 0.0f, props.min, props.max);

		add_strip(db, out_aa_start, out_start, _reuse_aa_outer_path.size(), false);
	}

	inline void builder::add_filled_rect_rounding_outline(const rect_props& props)
	{
		draw_buffer*	db	  = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);
		stroke_props&	out_p = _strokes[props.widget_id];
		rounding_props& rp	  = _roundings[props.widget_id];

		_reuse_outer_path.resize_explicit(0);
		_reuse_outline_path.resize_explicit(0);
		_reuse_aa_outer_path.resize_explicit(0);

		generate_rounded_rect(_reuse_outer_path, props.min, props.max, rp.rounding, rp.segments);
		generate_offset_rect(_reuse_outline_path, _reuse_outer_path, -static_cast<float>(out_p.thickness));

		const unsigned int out_start	 = db->vertex_count;
		const unsigned int central_start = out_start + _reuse_outer_path.size();

		if (props.multi_color)
		{
			add_vertices_multicolor(db, _reuse_outer_path, props.color_start, props.color_end, props.color_direction, props.min, props.max);
			add_central_vertex_multicolor(db, props.color_start, props.color_end, props.min, props.max);
		}
		else
		{
			add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
			add_central_vertex(db, props.color_start, props.min, props.max);
		}

		add_filled_rect_central(db, out_start, central_start, _reuse_outer_path.size());
		unsigned int outline_start = 0;

		// add original vertices
		const unsigned int copy_start = db->vertex_count;
		add_vertices(db, _reuse_outer_path, out_p.color, props.min, props.max);

		outline_start = db->vertex_count;
		add_vertices(db, _reuse_outline_path, out_p.color, props.min, props.max);
		add_strip(db, outline_start, copy_start, _reuse_outline_path.size(), false);
	}

	inline void builder::add_filled_rect_aa_outline_rounding(const rect_props& props)
	{
		draw_buffer*	db	  = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);
		aa_props&		aa_p  = _aa_props[props.widget_id];
		stroke_props&	out_p = _strokes[props.widget_id];
		rounding_props& rp	  = _roundings[props.widget_id];

		_reuse_outer_path.resize_explicit(0);
		_reuse_outline_path.resize_explicit(0);
		_reuse_aa_outer_path.resize_explicit(0);

		generate_rounded_rect(_reuse_outer_path, props.min, props.max, rp.rounding, rp.segments);

		generate_offset_rect(_reuse_outline_path, _reuse_outer_path, -static_cast<float>(out_p.thickness));

		generate_offset_rect(_reuse_aa_outer_path, _reuse_outline_path, -static_cast<float>(aa_p.thickness));

		const unsigned int out_start	 = db->vertex_count;
		const unsigned int central_start = out_start + _reuse_outer_path.size();

		if (props.multi_color)
		{
			add_vertices_multicolor(db, _reuse_outer_path, props.color_start, props.color_end, props.color_direction, props.min, props.max);
			add_central_vertex_multicolor(db, props.color_start, props.color_end, props.min, props.max);
		}
		else
		{
			add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
			add_central_vertex(db, props.color_start, props.min, props.max);
		}

		add_filled_rect_central(db, out_start, central_start, _reuse_outer_path.size());

		unsigned int outline_start = 0;

		// add original vertices
		const unsigned int copy_start = db->vertex_count;
		add_vertices(db, _reuse_outer_path, out_p.color, props.min, props.max);

		outline_start = db->vertex_count;
		add_vertices(db, _reuse_outline_path, out_p.color, props.min, props.max);
		add_strip(db, outline_start, copy_start, _reuse_outline_path.size(), false);

		const unsigned int out_aa_start = db->vertex_count;
		add_vertices_aa(db, _reuse_aa_outer_path, outline_start, 0.0f, props.min, props.max);
		add_strip(db, out_aa_start, outline_start, _reuse_aa_outer_path.size(), false);
	}

	void builder::add_stroke_rect(const rect_props& props)
	{
		draw_buffer* db = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);

		stroke_props& out_p = _strokes[props.widget_id];

		_reuse_outer_path.resize_explicit(0);
		_reuse_inner_path.resize_explicit(0);

		generate_sharp_rect(_reuse_outer_path, props.min, props.max);
		generate_offset_rect_4points(_reuse_inner_path, props.min, props.max, static_cast<float>(out_p.thickness));

		// Original stroke
		const unsigned int out_start = db->vertex_count;
		add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
		const unsigned int in_start = db->vertex_count;
		add_vertices(db, _reuse_inner_path, props.color_start, props.min, props.max);
		add_strip(db, out_start, in_start, _reuse_outer_path.size(), false);
	}

	void builder::add_stroke_rect_aa(const rect_props& props)
	{
		draw_buffer* db = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);
		_reuse_outer_path.resize_explicit(0);
		_reuse_inner_path.resize_explicit(0);
		_reuse_aa_outer_path.resize_explicit(0);
		_reuse_aa_inner_path.resize_explicit(0);

		stroke_props& out_p = _strokes[props.widget_id];
		aa_props&	  aa_p	= _aa_props[props.widget_id];

		generate_sharp_rect(_reuse_outer_path, props.min, props.max);
		generate_offset_rect(_reuse_inner_path, _reuse_outer_path, static_cast<float>(out_p.thickness));

		generate_offset_rect(_reuse_aa_outer_path, _reuse_outer_path, -static_cast<float>(aa_p.thickness));
		if (!_reuse_inner_path.empty())
		{
			generate_offset_rect(_reuse_aa_inner_path, _reuse_inner_path, static_cast<float>(aa_p.thickness));
		}

		// Original stroke
		const unsigned int out_start = db->vertex_count;
		add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
		const unsigned int in_start = db->vertex_count;
		add_vertices(db, _reuse_inner_path, props.color_start, props.min, props.max);
		add_strip(db, out_start, in_start, _reuse_outer_path.size(), false);
		// outer aa
		const unsigned int out_aa_start = db->vertex_count;
		add_vertices_aa(db, _reuse_aa_outer_path, out_start, 0.0f, props.min, props.max);
		add_strip(db, out_aa_start, out_start, _reuse_aa_outer_path.size(), false);

		// inner aa
		const unsigned int in_aa_start = db->vertex_count;
		add_vertices_aa(db, _reuse_aa_inner_path, in_start, 0.0f, props.min, props.max);
		add_strip(db, in_start, in_aa_start, _reuse_aa_inner_path.size(), false);
	}

	void builder::add_stroke_rect_rounding(const rect_props& props)
	{
		draw_buffer* db = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);
		_reuse_outer_path.resize_explicit(0);
		_reuse_inner_path.resize_explicit(0);
		stroke_props&	out_p = _strokes[props.widget_id];
		rounding_props& rp	  = _roundings[props.widget_id];

		generate_rounded_rect(_reuse_outer_path, props.min, props.max, rp.rounding, rp.segments);
		generate_rounded_rect(_reuse_inner_path, props.min + VEKT_VEC2(out_p.thickness, out_p.thickness), props.max - VEKT_VEC2(out_p.thickness, out_p.thickness), rp.rounding, rp.segments);

		_reuse_aa_outer_path.resize_explicit(0);
		_reuse_aa_inner_path.resize_explicit(0);

		// Original stroke
		const unsigned int out_start = db->vertex_count;
		add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
		const unsigned int in_start = db->vertex_count;
		add_vertices(db, _reuse_inner_path, props.color_start, props.min, props.max);
		add_strip(db, out_start, in_start, _reuse_outer_path.size(), false);
	}

	void builder::add_stroke_rect_aa_rounding(const rect_props& props)
	{
		draw_buffer*	db	  = get_draw_buffer(props.gfx.draw_order, props.gfx.user_data);
		stroke_props&	out_p = _strokes[props.widget_id];
		rounding_props& rp	  = _roundings[props.widget_id];
		aa_props&		aa_p  = _aa_props[props.widget_id];

		_reuse_outer_path.resize_explicit(0);
		_reuse_inner_path.resize_explicit(0);

		_reuse_aa_outer_path.resize_explicit(0);
		_reuse_aa_inner_path.resize_explicit(0);

		generate_rounded_rect(_reuse_outer_path, props.min, props.max, rp.rounding, rp.segments);
		generate_rounded_rect(_reuse_inner_path, props.min + VEKT_VEC2(out_p.thickness, out_p.thickness), props.max - VEKT_VEC2(out_p.thickness, out_p.thickness), rp.rounding, rp.segments);

		generate_offset_rect(_reuse_aa_outer_path, _reuse_outer_path, -static_cast<float>(aa_p.thickness));
		if (!_reuse_inner_path.empty())
		{
			generate_offset_rect(_reuse_aa_inner_path, _reuse_inner_path, static_cast<float>(aa_p.thickness));
		}

		// Original stroke
		const unsigned int out_start = db->vertex_count;
		add_vertices(db, _reuse_outer_path, props.color_start, props.min, props.max);
		const unsigned int in_start = db->vertex_count;
		add_vertices(db, _reuse_inner_path, props.color_start, props.min, props.max);
		add_strip(db, out_start, in_start, _reuse_outer_path.size(), false);

		// outer aa
		const unsigned int out_aa_start = db->vertex_count;
		add_vertices_aa(db, _reuse_aa_outer_path, out_start, 0.0f, props.min, props.max);
		add_strip(db, out_aa_start, out_start, _reuse_aa_outer_path.size(), false);

		// inner aa
		const unsigned int in_aa_start = db->vertex_count;
		add_vertices_aa(db, _reuse_aa_inner_path, in_start, 0.0f, props.min, props.max);
		add_strip(db, in_start, in_aa_start, _reuse_aa_inner_path.size(), false);
	}

	void builder::add_text(const text_props& text, const VEKT_VEC4& color, const VEKT_VEC2& position, const VEKT_VEC2& size, unsigned int draw_order, void* user_data)
	{
		if (text.font == nullptr)
		{
			V_ERR("vekt::builder::add_text() -> No font is set!");
			return;
		}

		draw_buffer* db = get_draw_buffer(draw_order, user_data, text.font);

		const float pixel_scale = text.font->_scale;
		const float subpixel	= text.font->type == font_type::lcd ? 3.0f : 1.0f;

		const unsigned int start_vertices_idx = db->vertex_count;
		const unsigned int start_indices_idx  = db->index_count;

#ifdef VEKT_STRING_CSTR
		const unsigned int char_count = static_cast<unsigned int>(strlen(text.text));
#else
		const unsigned int char_count = static_cast<unsigned int>(text.text.size());
#endif
		unsigned int vtx_counter = 0;

		VEKT_VEC2 pen = position;

		vertex* vertices = db->add_get_vertex(char_count * 4);
		index*	indices	 = db->add_get_index(char_count * 6);

		unsigned int current_char = 0;

		const float scale	= text.scale * pixel_scale;
		const float spacing = static_cast<float>(text.spacing) * scale;

		auto draw_char = [&](const glyph& g, unsigned long c, unsigned long previous_char) {
			if (previous_char != 0)
			{
				pen.x += static_cast<float>(text.font->glyph_info[previous_char].kern_advance[c]) * scale;
			}

			const float quad_left	= pen.x + g.x_offset / subpixel * text.scale;
			const float quad_top	= pen.y + g.y_offset * text.scale;
			const float quad_right	= quad_left + g.width * text.scale;
			const float quad_bottom = quad_top + g.height * text.scale;

			vertex& v0 = vertices[current_char * 4];
			vertex& v1 = vertices[current_char * 4 + 1];
			vertex& v2 = vertices[current_char * 4 + 2];
			vertex& v3 = vertices[current_char * 4 + 3];

			const float		uv_x = g.uv_x, uv_y = g.uv_y, uv_w = g.uv_w, uv_h = g.uv_h;
			const VEKT_VEC2 uv0(uv_x, uv_y);
			const VEKT_VEC2 uv1(uv_x + uv_w, uv_y);
			const VEKT_VEC2 uv2(uv_x + uv_w, uv_y + uv_h);
			const VEKT_VEC2 uv3(uv_x, uv_y + uv_h);

			v0.pos = {quad_left, quad_top};
			v1.pos = {quad_right, quad_top};
			v2.pos = {quad_right, quad_bottom};
			v3.pos = {quad_left, quad_bottom};

			v0.color = color;
			v1.color = color;
			v2.color = color;
			v3.color = color;

			v0.uv = uv0;
			v1.uv = uv1;
			v2.uv = uv2;
			v3.uv = uv3;

			indices[current_char * 6]	  = start_vertices_idx + vtx_counter;
			indices[current_char * 6 + 1] = start_vertices_idx + vtx_counter + 1;
			indices[current_char * 6 + 2] = start_vertices_idx + vtx_counter + 3;

			indices[current_char * 6 + 3] = start_vertices_idx + vtx_counter + 1;
			indices[current_char * 6 + 4] = start_vertices_idx + vtx_counter + 2;
			indices[current_char * 6 + 5] = start_vertices_idx + vtx_counter + 3;

			vtx_counter += 4;

			pen.x += g.advance_x * scale + spacing;
			current_char++;
		};

#ifdef VEKT_STRING_CSTR
		const char* cstr = text.text;
#else
		const char* cstr = text.text.c_str();
#endif
		const uint8_t* c;
		float		   max_y_offset = 0;
		for (c = (uint8_t*)cstr; *c; c++)
		{
			auto		 character = *c;
			const glyph& ch		   = text.font->glyph_info[character];
			max_y_offset		   = math::max(max_y_offset, -ch.y_offset);
		}
		// pen.y += 10;
		pen.y += (max_y_offset * text.scale);

		unsigned long previous_char = 0;
		for (c = (uint8_t*)cstr; *c; c++)
		{
			auto		 character = *c;
			const glyph& ch		   = text.font->glyph_info[character];
			draw_char(ch, character, previous_char);
			previous_char = character;
		}
	}

	void builder::add_text_cached(const text_props& text, const VEKT_VEC4& color, const VEKT_VEC2& position, const VEKT_VEC2& size, unsigned int draw_order, void* user_data)
	{
		if (text.font == nullptr)
		{
			V_ERR("vekt::builder::add_text() -> No font is set!");
			return;
		}

		draw_buffer*   db	= get_draw_buffer(draw_order, user_data, text.font);
		const uint64_t hash = text.hash == 0 ? text_cache::hash_text_props(text, color) : text.hash;
		auto		   it	= _text_cache.find([hash](const text_cache& cache) -> bool { return cache.hash == hash; });
		if (it != _text_cache.end())
		{
			const unsigned int start_vtx = db->vertex_count;

			const unsigned int idx_count = it->vtx_count / 2 * 3;
			vertex*			   vertices	 = db->add_get_vertex(it->vtx_count);
			index*			   indices	 = db->add_get_index(idx_count);
			MEMCPY(vertices, &_text_cache_vertex_buffer[it->vtx_start], it->vtx_count * sizeof(vertex));
			MEMCPY(indices, &_text_cache_index_buffer[it->idx_start], idx_count * sizeof(index));

			for (unsigned int i = 0; i < it->vtx_count; i++)
			{
				vertices[i].pos.x += position.x;
				vertices[i].pos.y += position.y;
			}

			for (unsigned int i = 0; i < idx_count; i++)
			{
				indices[i] += start_vtx;
			}

			return;
		}

		const float pixel_scale = text.font->_scale;
		const float subpixel	= text.font->type == font_type::lcd ? 3.0f : 1.0f;

		const unsigned int start_vertices_idx = db->vertex_count;
		const unsigned int start_indices_idx  = db->index_count;

#ifdef VEKT_STRING_CSTR
		const unsigned int char_count = static_cast<unsigned int>(strlen(text.text));
#else
		const unsigned int char_count = static_cast<unsigned int>(text.text.size());
#endif
		unsigned int vtx_counter = 0;

		VEKT_VEC2 pen = VEKT_VEC2();

		vertex* vertices = db->add_get_vertex(char_count * 4);
		index*	indices	 = db->add_get_index(char_count * 6);

		unsigned int current_char = 0;

		const float scale	= text.scale * pixel_scale;
		const float spacing = static_cast<float>(text.spacing) * scale;

		auto draw_char = [&](const glyph& g, unsigned long c, unsigned long previous_char) {
			if (previous_char != 0)
			{
				pen.x += static_cast<float>(text.font->glyph_info[previous_char].kern_advance[c]) * scale;
			}

			const float quad_left	= pen.x + g.x_offset / subpixel * text.scale;
			const float quad_top	= pen.y + g.y_offset * text.scale;
			const float quad_right	= quad_left + g.width * text.scale;
			const float quad_bottom = quad_top + g.height * text.scale;

			vertex& v0 = vertices[current_char * 4];
			vertex& v1 = vertices[current_char * 4 + 1];
			vertex& v2 = vertices[current_char * 4 + 2];
			vertex& v3 = vertices[current_char * 4 + 3];

			const float		uv_x = g.uv_x, uv_y = g.uv_y, uv_w = g.uv_w, uv_h = g.uv_h;
			const VEKT_VEC2 uv0(uv_x, uv_y);
			const VEKT_VEC2 uv1(uv_x + uv_w, uv_y);
			const VEKT_VEC2 uv2(uv_x + uv_w, uv_y + uv_h);
			const VEKT_VEC2 uv3(uv_x, uv_y + uv_h);

			v0.pos = {quad_left, quad_top};
			v1.pos = {quad_right, quad_top};
			v2.pos = {quad_right, quad_bottom};
			v3.pos = {quad_left, quad_bottom};

			v0.color = color;
			v1.color = color;
			v2.color = color;
			v3.color = color;

			v0.uv = uv0;
			v1.uv = uv1;
			v2.uv = uv2;
			v3.uv = uv3;

			indices[current_char * 6]	  = vtx_counter;
			indices[current_char * 6 + 1] = vtx_counter + 1;
			indices[current_char * 6 + 2] = vtx_counter + 3;

			indices[current_char * 6 + 3] = vtx_counter + 1;
			indices[current_char * 6 + 4] = vtx_counter + 2;
			indices[current_char * 6 + 5] = vtx_counter + 3;

			vtx_counter += 4;

			pen.x += g.advance_x * scale + spacing;
			current_char++;
		};

#ifdef VEKT_STRING_CSTR
		const char* cstr = text.text;
#else
		const char* cstr = text.text.c_str();
#endif
		const uint8_t* c;
		float		   max_y_offset = 0;
		for (c = (uint8_t*)cstr; *c; c++)
		{
			auto		 character = *c;
			const glyph& ch		   = text.font->glyph_info[character];
			max_y_offset		   = math::max(max_y_offset, -ch.y_offset);
		}

		pen.y += max_y_offset * text.scale;

		unsigned long previous_char = 0;
		for (c = (uint8_t*)cstr; *c; c++)
		{
			auto		 character = *c;
			const glyph& ch		   = text.font->glyph_info[character];
			draw_char(ch, character, previous_char);
			previous_char = character;
		}

		const unsigned int idx_count = vtx_counter / 2 * 3;

		if (_text_cache_vertex_count + vtx_counter < _text_cache_vertex_size && _text_cache_index_count + idx_count < _text_cache_index_size)
		{
			text_cache cache;
			cache.hash		= hash;
			cache.vtx_count = vtx_counter;
			cache.vtx_start = _text_cache_vertex_count;
			cache.idx_start = _text_cache_index_count;
			_text_cache.push_back(cache);
			MEMCPY(&_text_cache_vertex_buffer[_text_cache_vertex_count], vertices, vtx_counter * sizeof(vertex));
			MEMCPY(&_text_cache_index_buffer[_text_cache_index_count], indices, idx_count * sizeof(index));
			_text_cache_vertex_count += vtx_counter;
			_text_cache_index_count += idx_count;
		}

		for (unsigned int i = 0; i < vtx_counter; i++)
		{
			vertices[i].pos.x += position.x;
			vertices[i].pos.y += position.y;
		}

		for (unsigned int i = 0; i < idx_count; i++)
		{
			indices[i] += start_vertices_idx;
		}
	}

	VEKT_VEC2 builder::get_text_size(const text_props& text, const VEKT_VEC2& parent_size)
	{
		if (text.font == nullptr)
		{
			V_ERR("vekt::builder::get_text_size() -> No font is set!");
			return VEKT_VEC2();
		}

		const font* fnt			= text.font;
		const float pixel_scale = fnt->_scale;

		float total_x = 0.0f;
		float max_y	  = 0.0f;

		// if (!math::equals(text._parent_relative_scale, 0.0f, 0.001f) && (!math::equals(parent_size.x, 0.0f, 0.001f) || !math::equals(parent_size.y, 0.0f, 0.001f)))
		// {
		// 	const float min	  = math::min(parent_size.x, parent_size.y) * text._parent_relative_scale;
		// 	const float ratio = min / static_cast<float>(fnt->size);
		// 	text.scale		  = ratio;
		// }

		const float used_scale = text.scale;
#ifdef VEKT_STRING_CSTR
		const char* str = text.text;
#else
		const char* str = text.text.c_str();
#endif
		const float spacing = static_cast<float>(text.spacing) * used_scale;
		const float scale	= pixel_scale * used_scale;

		for (size_t i = 0; str[i]; ++i)
		{
			const uint8_t c0 = static_cast<uint8_t>(str[i]);
			const glyph&  g0 = fnt->glyph_info[c0];

			total_x += g0.advance_x * scale;

			if (str[i + 1])
			{
				const uint8_t c1 = static_cast<uint8_t>(str[i + 1]);
				total_x += g0.kern_advance[c1] * scale;
			}

			total_x += spacing;
			max_y = math::max(max_y, static_cast<float>(g0.height) * used_scale);
		}

		return VEKT_VEC2(total_x - spacing, max_y);
	}

	void builder::add_strip(draw_buffer* db, unsigned int outer_start, unsigned int inner_start, unsigned int size, bool add_ccw)
	{
		index* idx = db->add_get_index(size * 6);

		for (unsigned int i = 0; i < size; i++)
		{
			const unsigned int p1_curr = outer_start + i;
			const unsigned int p1_next = outer_start + (i + 1) % size;
			const unsigned int p2_curr = inner_start + i;
			const unsigned int p2_next = inner_start + (i + 1) % size;
			const unsigned int base	   = i * 6;
			idx[base]				   = p1_curr;

			if (add_ccw)
			{
				idx[base + 1] = p2_curr;
				idx[base + 2] = p1_next;
			}
			else
			{
				idx[base + 1] = p1_next;
				idx[base + 2] = p2_curr;
			}

			idx[base + 3] = p1_next;
			if (add_ccw)
			{
				idx[base + 4] = p2_curr;
				idx[base + 5] = p2_next;
			}
			else
			{
				idx[base + 4] = p2_next;
				idx[base + 5] = p2_curr;
			}
		}
	}

	void builder::add_filled_rect(draw_buffer* db, unsigned int start)
	{
		index* idx = db->add_get_index(6);
		idx[0]	   = start;
		idx[1]	   = start + 1;
		idx[2]	   = start + 3;
		idx[3]	   = start + 1;
		idx[4]	   = start + 2;
		idx[5]	   = start + 3;
	}

	void builder::add_filled_rect_central(draw_buffer* db, unsigned int start, unsigned int central_start, unsigned int size)
	{
		for (unsigned int i = 0; i < size; i++)
		{
			db->add_index(central_start);
			db->add_index(start + i);
			db->add_index(start + ((i + 1) % (size)));
		}
	}

	void builder::add_vertices_aa(draw_buffer* db, const vector<VEKT_VEC2>& path, unsigned int original_vertices_idx, float alpha, const VEKT_VEC2& min, const VEKT_VEC2& max)
	{
		const unsigned int start_vtx_idx = db->vertex_count;

		for (unsigned int i = 0; i < path.size(); i++)
		{
			vertex& vtx = db->add_get_vertex();
			vtx.pos		= path[i];
			vtx.color	= db->vertex_start[original_vertices_idx + i].color;
			vtx.color.w = alpha;
			vtx.uv.x	= math::remap(vtx.pos.x, min.x, max.x, 0.0f, 1.0f);
			vtx.uv.y	= math::remap(vtx.pos.y, min.y, max.y, 0.0f, 1.0f);
		}
	}

	void builder::generate_offset_rect_4points(vector<VEKT_VEC2>& out_path, const VEKT_VEC2& min, const VEKT_VEC2& max, float amount)
	{
		out_path.resize_explicit(4);
		out_path[0] = {min.x + amount, min.y + amount}; // Top-Left
		out_path[1] = {max.x - amount, min.y + amount}; // Top-Right
		out_path[2] = {max.x - amount, max.y - amount}; // Bottom-Right
		out_path[3] = {min.x + amount, max.y - amount}; // Bottom-Left
	}

	void builder::generate_offset_rect(vector<VEKT_VEC2>& out_path, const vector<VEKT_VEC2>& base_path, float distance)
	{
		if (base_path.size() < 2)
			return;
		out_path.resize_explicit(base_path.size());

		const size_t num_points = base_path.size();

		for (size_t i = 0; i < num_points; ++i)
		{
			// Get the current, previous, and next points
			const VEKT_VEC2& p_curr = base_path[i];
			const VEKT_VEC2& p_prev = base_path[(i + num_points - 1) % num_points];
			const VEKT_VEC2& p_next = base_path[(i + 1) % num_points];

			const VEKT_VEC2 tangent1	 = (p_curr - p_prev).normalized();
			const VEKT_VEC2 tangent2	 = (p_next - p_curr).normalized();
			const VEKT_VEC2 normal1		 = {-tangent1.y, tangent1.x};
			const VEKT_VEC2 normal2		 = {-tangent2.y, tangent2.x};
			const VEKT_VEC2 miter_vector = (normal1 + normal2).normalized();

			// Calculate the offset vertex
			const VEKT_VEC2 path = p_curr + miter_vector * distance;
			out_path[i]			 = path;
		}
	}

	void builder::add_vertices(draw_buffer* db, const vector<VEKT_VEC2>& path, const VEKT_VEC4& color, const VEKT_VEC2& min, const VEKT_VEC2& max)
	{
		vertex*		vertices	= db->add_get_vertex(path.size());
		const float inv_x_range = 1.0f / (max.x - min.x);
		const float inv_y_range = 1.0f / (max.y - min.y);

		for (unsigned int i = 0; i < path.size(); i++)
		{
			vertex& vtx = vertices[i];
			vtx.pos		= path[i];
			vtx.color	= color;
			vtx.uv.x	= (vtx.pos.x - min.x) * inv_x_range;
			vtx.uv.y	= (vtx.pos.y - min.y) * inv_y_range;
		}
	}

	void builder::add_vertices_multicolor(draw_buffer* db, const vector<VEKT_VEC2>& path, const VEKT_VEC4& color_start, const VEKT_VEC4& color_end, direction direction, const VEKT_VEC2& min, const VEKT_VEC2& max)
	{
		const unsigned int start_vtx_idx = db->vertex_count;

		vertex* vertices = db->add_get_vertex(path.size());

		const float inv_x_range = 1.0f / (max.x - min.x);
		const float inv_y_range = 1.0f / (max.y - min.y);

		const float inv_color_remap_x_range = 1.0f / (max.x - min.x);
		const float inv_color_remap_y_range = 1.0f / (max.y - min.y);

		const VEKT_VEC4 color_diff = color_end - color_start;

		for (unsigned int i = 0; i < path.size(); i++)
		{
			vertex& vtx = vertices[i];
			vtx.pos		= path[i];

			float ratio;
			if (direction == direction::horizontal)
				ratio = (vtx.pos.x - min.x) * inv_color_remap_x_range;
			else
				ratio = (vtx.pos.y - min.y) * inv_color_remap_y_range;

			vtx.color.x = color_start.x + color_diff.x * ratio;
			vtx.color.y = color_start.y + color_diff.y * ratio;
			vtx.color.z = color_start.z + color_diff.z * ratio;
			vtx.color.w = color_start.w + color_diff.w * ratio;

			// const float ratio = direction == direction::horizontal ? math::remap(vtx.pos.x, min.x, max.x, 0.0f, 1.0f) : math::remap(vtx.pos.y, min.y, max.y, 0.0f, 1.0f);
			// vtx.color.x		  = math::lerp(color_start.x, color_end.x, ratio);
			// vtx.color.y		  = math::lerp(color_start.y, color_end.y, ratio);
			// vtx.color.z		  = math::lerp(color_start.z, color_end.z, ratio);
			// vtx.color.w		  = math::lerp(color_start.w, color_end.w, ratio);
			vtx.uv.x = (vtx.pos.x - min.x) * inv_x_range;
			vtx.uv.y = (vtx.pos.y - min.y) * inv_y_range;
		}
	}

	void builder::add_central_vertex(draw_buffer* db, const VEKT_VEC4& color, const VEKT_VEC2& min, const VEKT_VEC2& max)
	{
		vertex& vtx = db->add_get_vertex();
		vtx.pos		= (min + max) * 0.5f;
		vtx.color	= color;
		vtx.uv		= VEKT_VEC2(0.5f, 0.5f);
	}

	void builder::add_central_vertex_multicolor(draw_buffer* db, const VEKT_VEC4& color_start, const VEKT_VEC4& color_end, const VEKT_VEC2& min, const VEKT_VEC2& max)
	{
		vertex& vtx = db->add_get_vertex();
		vtx.pos		= (min + max) * 0.5f;
		vtx.color	= (color_start + color_end) * 0.5f;
		vtx.uv		= VEKT_VEC2(0.5f, 0.5f);
	}

	void builder::generate_rounded_rect(vector<VEKT_VEC2>& out_path, const VEKT_VEC2& min, const VEKT_VEC2& max, float r, int segments)
	{
		r = math::min(r, math::min((max.x - min.x) * 0.5f, (max.y - min.y) * 0.5f)); // Clamp radius

		if (segments == 0)
			segments = 10;

		segments = math::min(math::max(1, segments), 90);

		// top left
		{
			const VEKT_VEC2 center = VEKT_VEC2(min.x + r, min.y + r);
			for (int i = 0; i <= segments; ++i)
			{

				const float		target_angle  = DEG_2_RAD * (270.0f + (90.0f / segments) * i);
				const VEKT_VEC2 point_on_unit = VEKT_VEC2(math::sin(target_angle), -math::cos(target_angle)) * r;
				const VEKT_VEC2 point		  = center + point_on_unit;
				out_path.push_back(point);
			}
		}
		// top right
		{
			const VEKT_VEC2 center = VEKT_VEC2(max.x - r, min.y + r);
			for (int i = 0; i <= segments; ++i)
			{

				const float		target_angle  = DEG_2_RAD * ((90.0f / segments) * i);
				const VEKT_VEC2 point_on_unit = VEKT_VEC2(math::sin(target_angle), -math::cos(target_angle)) * r;
				const VEKT_VEC2 point		  = center + point_on_unit;
				out_path.push_back(point);
			}
		}

		// bottom right
		{
			const VEKT_VEC2 center = VEKT_VEC2(max.x - r, max.y - r);
			for (int i = 0; i <= segments; ++i)
			{

				const float		target_angle  = DEG_2_RAD * (90.0f + (90.0f / segments) * i);
				const VEKT_VEC2 point_on_unit = VEKT_VEC2(math::sin(target_angle), -math::cos(target_angle)) * r;
				const VEKT_VEC2 point		  = center + point_on_unit;
				out_path.push_back(point);
			}
		}

		// bottom left
		{
			const VEKT_VEC2 center = VEKT_VEC2(min.x + r, max.y - r);
			for (int i = 0; i <= segments; ++i)
			{

				const float		target_angle  = DEG_2_RAD * (180.0f + (90.0f / segments) * i);
				const VEKT_VEC2 point_on_unit = VEKT_VEC2(math::sin(target_angle), -math::cos(target_angle)) * r;
				const VEKT_VEC2 point		  = center + point_on_unit;
				out_path.push_back(point);
			}
		}
	}

	void builder::generate_sharp_rect(vector<VEKT_VEC2>& out_path, const VEKT_VEC2& min, const VEKT_VEC2& max)
	{
		const unsigned int sz = out_path.size();
		out_path.resize_explicit(sz + 4);
		out_path[sz]	 = {min.x, min.y}; // Top-left
		out_path[sz + 1] = {max.x, min.y}; // Top-right
		out_path[sz + 2] = {max.x, max.y}; // Bottom-right
		out_path[sz + 3] = {min.x, max.y}; // Bottom-left
	}

	draw_buffer* builder::get_draw_buffer(unsigned int draw_order, void* user_data, font* fnt)
	{
		const VEKT_VEC4& clip = get_current_clip();

		for (draw_buffer& db : _draw_buffers)
		{
			if (db.clip.equals(clip, 0.9f) && db.draw_order == draw_order && db.user_data == user_data && db.used_font == fnt)
			{
				return &db;
			}
		}

		ASSERT(_buffer_counter < _buffer_count);

		draw_buffer db	 = {};
		db.clip			 = clip;
		db.draw_order	 = draw_order;
		db.user_data	 = user_data;
		db.vertex_start	 = _vertex_buffer + _buffer_counter * _vertex_count_per_buffer;
		db.index_start	 = _index_buffer + _buffer_counter * _index_count_per_buffer;
		db.used_font	 = fnt;
		db._max_vertices = _vertex_count_per_buffer;
		db._max_indices	 = _index_count_per_buffer;

		_buffer_counter++;
		_draw_buffers.push_back(db);
		return &_draw_buffers.get_back();
	}

	VEKT_VEC4 builder::calculate_intersection(const VEKT_VEC4& r1, const VEKT_VEC4& r2) const
	{
		const float x	   = math::max(r1.x, r2.x);
		const float y	   = math::max(r1.y, r2.y);
		const float right  = math::min(r1.x + r1.z, r2.x + r2.z);
		const float bottom = math::min(r1.y + r1.w, r2.y + r2.w);

		if (right < x || bottom < y)
		{
			return VEKT_VEC4();
		}
		return {x, y, right - x, bottom - y};
	}

	void builder::widget_add_debug_wrap(id widget)
	{
		id wrapper = allocate();
		widget_set_pos(wrapper, VEKT_VEC2());
		widget_set_size(wrapper, VEKT_VEC2(1.0f, 1.0f));

		widget_gfx& r = widget_get_gfx(wrapper);
		r.flags |= gfx_is_stroke;
		_strokes[wrapper].thickness = 1;
		r.color						= VEKT_VEC4(1, 0, 0, 1);
		widget_add_child(widget, wrapper);
	}

	////////////////////////////////////////////////////////////////////////////////
	// :: ATLAS IMPL
	////////////////////////////////////////////////////////////////////////////////

	atlas::atlas(unsigned int width, unsigned int height, bool is_lcd)
	{
		_width	= width;
		_height = height;
		_is_lcd = is_lcd;

		_available_slices.push_back(new atlas::slice(0, _height));
		_data_size		= width * height * (is_lcd ? 3 : 1);
		const size_t sz = static_cast<size_t>(_data_size);
		_data			= reinterpret_cast<unsigned char*>(MALLOC(sz));
		memset(_data, 0, sz);
	}

	atlas::~atlas()
	{
		for (slice* slc : _available_slices)
			delete slc;
		_available_slices.clear();

		FREE(_data);
	}

	bool atlas::add_font(font* font)
	{
		if (font->_atlas_required_height > _height)
			return false;

		unsigned int best_slice_diff = _height;
		slice*		 best_slice		 = nullptr;

		for (slice* slc : _available_slices)
		{
			if (slc->height < font->_atlas_required_height)
				continue;

			const unsigned int diff = slc->height - font->_atlas_required_height;
			if (diff < best_slice_diff)
			{
				best_slice_diff = diff;
				best_slice		= slc;
			}
		}

		if (best_slice == nullptr)
			return false;

		font->_atlas	 = this;
		font->_atlas_pos = best_slice->pos;
		_fonts.push_back(font);

		best_slice->pos += font->_atlas_required_height;
		best_slice->height -= font->_atlas_required_height;

		if (best_slice->height == 0)
		{
			_available_slices.remove(best_slice);
			delete best_slice;
		}

		return true;
	}

	void atlas::remove_font(font* fnt)
	{
		slice* slc = new slice(fnt->_atlas_pos, fnt->_atlas_pos);
		_available_slices.push_back(slc);
		_fonts.remove(fnt);
	}

	void font_manager::find_atlas(font* fnt)
	{
		for (atlas* atl : _atlases)
		{
			if (atl->get_is_lcd() != (fnt->type == font_type::lcd))
				continue;
			if (atl->add_font(fnt))
			{
				return;
			}
		}

		atlas* atl = new atlas(config.atlas_width, config.atlas_height, fnt->type == font_type::lcd);
		_atlases.push_back(atl);
		if (_atlas_created_cb)
			_atlas_created_cb(atl);
		const bool ok = atl->add_font(fnt);
		ASSERT(ok);
	}

	font* font_manager::load_font(const unsigned char* data, unsigned int data_size, unsigned int size, unsigned int range0, unsigned int range1, font_type type, int sdf_padding, int sdf_edge, float sdf_distance)
	{

		stbtt_fontinfo stb_font;
		stbtt_InitFont(&stb_font, data, stbtt_GetFontOffsetForIndex(data, 0));

		font* fnt	= new font();
		fnt->_scale = stbtt_ScaleForMappingEmToPixels(&stb_font, static_cast<float>(size));
		fnt->type	= type;

		stbtt_GetFontVMetrics(&stb_font, &fnt->ascent, &fnt->descent, &fnt->line_gap);
		fnt->size = size;

		int		  total_width = 0;
		int		  max_height  = 0;
		const int x_padding	  = 2;

		for (int i = range0; i < range1; i++)
		{
			glyph& glyph_info = fnt->glyph_info[i];

			if (type == font_type::sdf)
			{
				int x_off, y_off;
				glyph_info.sdf_data = stbtt_GetCodepointSDF(&stb_font, fnt->_scale, i, sdf_padding, sdf_edge, sdf_distance, &glyph_info.width, &glyph_info.height, &x_off, &y_off);
				glyph_info.x_offset = static_cast<float>(x_off);
				glyph_info.y_offset = static_cast<float>(y_off);
			}
			else if (type == font_type::lcd)
			{
				int ix0 = 0, iy0 = 0, ix1 = 0, iy1 = 0;
				stbtt_GetCodepointBitmapBoxSubpixel(&stb_font, i, fnt->_scale * 3, fnt->_scale, 1.0f, 0.0f, &ix0, &iy0, &ix1, &iy1);
				glyph_info.width	= ix1 - ix0;
				glyph_info.height	= iy1 - iy0;
				glyph_info.x_offset = static_cast<float>(ix0);
				glyph_info.y_offset = static_cast<float>(iy0);
			}
			else
			{
				int ix0 = 0, iy0 = 0, ix1 = 0, iy1 = 0;
				stbtt_GetCodepointBitmapBox(&stb_font, i, fnt->_scale, fnt->_scale, &ix0, &iy0, &ix1, &iy1);
				glyph_info.width	= ix1 - ix0;
				glyph_info.height	= iy1 - iy0;
				glyph_info.x_offset = static_cast<float>(ix0);
				glyph_info.y_offset = static_cast<float>(iy0);
			}

			if (glyph_info.width >= 1)
				total_width += glyph_info.width + x_padding;
			max_height = static_cast<int>(math::max(max_height, glyph_info.height));
			stbtt_GetCodepointHMetrics(&stb_font, i, &glyph_info.advance_x, &glyph_info.left_bearing);

			for (int j = 0; j < 128; j++)
				glyph_info.kern_advance[j] = stbtt_GetCodepointKernAdvance(&stb_font, i, j);
		}

		const int required_rows		= static_cast<int>(math::ceilf(static_cast<float>(total_width) / static_cast<float>(config.atlas_width)));
		const int required_height	= max_height;
		fnt->_atlas_required_height = required_rows * required_height;
		find_atlas(fnt);

		if (fnt->_atlas == nullptr)
		{
			delete fnt;
			V_ERR("vekt::font_manager::load_font -> Failed finding an atlas for the font!");
			return nullptr;
		}

		_fonts.push_back(fnt);

		int current_atlas_pen_x = 0;
		int current_atlas_pen_y = 0;

		for (int i = range0; i < range1; i++)
		{
			glyph& glyph_info = fnt->glyph_info[i];

			if (glyph_info.width <= 0 || glyph_info.height <= 0)
			{
				glyph_info.atlas_x = 0;
				glyph_info.atlas_y = 0;
				continue;
			}

			const int w = glyph_info.width;
			const int h = glyph_info.height;
			if (current_atlas_pen_x + w > static_cast<int>(config.atlas_width))
			{
				current_atlas_pen_x = 0;
				current_atlas_pen_y += max_height;
			}

			if (current_atlas_pen_y + h > static_cast<int>(fnt->_atlas_required_height))
			{
				ASSERT(false);
			}

			glyph_info.atlas_x = current_atlas_pen_x;
			glyph_info.atlas_y = fnt->_atlas_pos + current_atlas_pen_y;
			glyph_info.uv_x	   = static_cast<float>(glyph_info.atlas_x) / static_cast<float>(fnt->_atlas->get_width());
			glyph_info.uv_y	   = static_cast<float>(glyph_info.atlas_y) / static_cast<float>(fnt->_atlas->get_height());
			glyph_info.uv_w	   = static_cast<float>(w) / static_cast<float>(fnt->_atlas->get_width());
			glyph_info.uv_h	   = static_cast<float>(h) / static_cast<float>(fnt->_atlas->get_height());

			const unsigned int pixel_size	  = fnt->type == font_type::lcd ? 3 : 1;
			unsigned char*	   dest_pixel_ptr = fnt->_atlas->get_data() + (glyph_info.atlas_y * fnt->_atlas->get_width() * pixel_size) + glyph_info.atlas_x * pixel_size;

			if (type == font_type::sdf)
			{
				int atlas_stride = fnt->_atlas->get_width(); // assuming 1 byte per pixel
				for (int row = 0; row < h; ++row)
				{
					std::memcpy(dest_pixel_ptr + row * atlas_stride, glyph_info.sdf_data + row * w, w);
				}
			}
			else if (type == font_type::lcd)
			{
				stbtt_MakeCodepointBitmapSubpixel(&stb_font, dest_pixel_ptr, w, h, fnt->_atlas->get_width() * 3, fnt->_scale * 3, fnt->_scale, 1.0f, 0.0f, i);
			}
			else
			{
				stbtt_MakeCodepointBitmap(&stb_font,
										  dest_pixel_ptr,
										  w,						// Output bitmap width
										  h,						// Output bitmap height
										  fnt->_atlas->get_width(), // Atlas stride/pitch
										  fnt->_scale,				// Horizontal scale
										  fnt->_scale,				// Vertical scale
										  i);						// Codepoint
			}

			current_atlas_pen_x += w + x_padding;
		}

		if (_atlas_updated_cb)
			_atlas_updated_cb(fnt->_atlas);
		return fnt;
	}

	font* font_manager::load_font(const char* filename, unsigned int size, unsigned int range_start, unsigned int range_end, font_type type, int sdf_padding, int sdf_edge, float sdf_distance)
	{
		if (range_start >= range_end)
		{
			V_ERR("vekt::font_manager::load_font -> range_start needs to be smaller than range_end! %s", filename);
			return nullptr;
		}

		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			V_ERR("vekt::font_manager::load_font -> Failed opening font file! %s", filename);
			return nullptr;
		}

		std::streamsize file_size = file.tellg();
		file.seekg(0, std::ios::beg);

		vector<unsigned char> ttf_buffer;
		ttf_buffer.resize(static_cast<unsigned int>(file_size));
		if (!file.read(reinterpret_cast<char*>(ttf_buffer.data()), file_size))
		{
			V_ERR("vekt::font_manager::load_font -> Failed reading font buffer! %s", filename);
			return nullptr;
		}
		return load_font(ttf_buffer.data(), ttf_buffer.size(), size, range_start, range_end, type, sdf_padding, sdf_edge, sdf_distance);
	}

	void font_manager::unload_font(font* fnt)
	{
		fnt->_atlas->remove_font(fnt);

		if (fnt->_atlas->empty())
		{
			_atlases.remove(fnt->_atlas);

			if (_atlas_destroyed_cb)
				_atlas_destroyed_cb(fnt->_atlas);

			delete fnt->_atlas;
		}

		_fonts.remove(fnt);
		delete fnt;
	}

	void font_manager::init()
	{
	}

	void font_manager::uninit()
	{
		for (atlas* atl : _atlases)
		{
			if (_atlas_destroyed_cb)
				_atlas_destroyed_cb(atl);
			delete atl;
		}

		for (font* fnt : _fonts)
			delete fnt;

		_atlases.clear();
		_fonts.clear();
	}

	font::~font()
	{
		for (unsigned int i = 0; i < 128; i++)
		{
			glyph& g = glyph_info[i];

			if (g.sdf_data)
				stbtt_FreeSDF(g.sdf_data, nullptr);
		}
	}
}

#endif
