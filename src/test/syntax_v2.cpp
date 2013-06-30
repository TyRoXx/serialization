#include <boost/test/unit_test.hpp>

#include <szn/struct2.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <szn/vector.hpp>

namespace szn
{
	namespace
	{
		struct TestStruct
		{
			typedef std::vector<int, std::allocator<int> > VectorInt;
			typedef szn::Vector<szn::BE8, szn::BE16> Vector8_16;

			RXN_REFLECT(
				(RXN_MEMBERS) (RXN_ITERATE),
				(a, int) (szn::BE16),
				(b, long) (szn::BE32),
				(c, std::string) (szn::bytes<szn::BE8>),
				(v, std::vector<int, std::allocator<int>>) (szn::Vector<szn::BE8, szn::BE16>)
			)
		};

#define TEST_STRUCT_2_MEMBERS \
		(name, std::string) (szn::bytes<szn::BE32>), \
		(id, int) (szn::BE16)

		struct TestStruct2
		{
			RXN_REFLECT((RXN_MEMBERS) (RXN_ITERATE), TEST_STRUCT_2_MEMBERS)
		};

		struct TestVisitor
		{
			template <class Format, class Value>
			void accept(Value &member)
			{
				(void)member;
			}
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v2)
	{
	//	static_assert(false, BOOST_PP_STRINGIZE((RXN_VISIT(name, (type), ((szn::BE8))))));
	//	static_assert(false, BOOST_PP_STRINGIZE((RXN_VISIT(name, (type), ((szn::Vector<szn::BE8, szn::BE16>))))));

		TestStruct t;
		t.a = 0;
		t.b = 3;
		t.c = "hallo";
		t.v.resize(2, 12);
		TestVisitor v;
		t.iterate(v);

		TestStruct2 t2;
		t2.name = "name";
		t2.id = 123;
	}

#if SZN_HAS_UNIQUE_PTR //TODO: avoid unique_ptr
	namespace gc
	{
		struct Object
		{
			virtual ~Object()
			{
			}

			bool is_marked() const
			{
				return m_is_marked;
			}

			void unmark()
			{
				m_is_marked = false;
			}

			void mark()
			{
				if (m_is_marked)
				{
					return;
				}
				m_is_marked = true;
				mark_children();
			}

		private:

			bool m_is_marked;

			virtual void mark_children() = 0;
		};

#define SZNTEST_MARK_CHILD(name, type, annotations) \
	RXN_REMOVE_PAREN(BOOST_PP_SEQ_ELEM(0, annotations))().handle_value(this->name);

#define SZNTEST_GC(fields) \
		private: virtual void mark_children() SZN_OVERRIDE { \
			BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, SZNTEST_MARK_CHILD, fields) \
		}

		struct NonManaged SZN_FINAL
		{
			template <class Value>
			void handle_value(Value const &) const
			{
			}
		};

		struct Managed SZN_FINAL
		{
			inline void handle_value(Object *object) const
			{
				if (object)
				{
					object->mark();
				}
			}
		};

#define SZNTEST_OBJECT(...) RXN_REFLECT((RXN_MEMBERS) (SZNTEST_GC), __VA_ARGS__)

		struct Pair : Object
		{
			SZNTEST_OBJECT(
						(first, Object *) (Managed),
						(second, Object *) (Managed)
						)
		};

		struct GarbageCollector SZN_FINAL
		{
			template <class Type>
			Object &createObject()
			{
				std::unique_ptr<Object> object(new Type);
				m_objects.push_back(std::move(object));
				return *m_objects.back();
			}

			void unmark()
			{
				BOOST_FOREACH (auto const &obj, m_objects)
				{
					obj->unmark();
				}
			}

			void sweep()
			{
				auto const new_end = std::partition(
							m_objects.begin(),
							m_objects.end(),
							[](std::unique_ptr<Object> const &obj)
				{
						return obj->is_marked();
				});
				m_objects.erase(new_end, m_objects.end());
			}

		private:

			std::vector<std::unique_ptr<Object>> m_objects;
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v2_GC)
	{
		gc::GarbageCollector gc;
		gc.unmark();
		gc.sweep();

		gc::Pair &p = dynamic_cast<gc::Pair &>(gc.createObject<gc::Pair>());
		p.first = &p;
		p.second = NULL;

		gc.unmark();
		p.mark();
		gc.sweep();

		p.second = &p;

		gc.unmark();
		gc.sweep();
	}
#endif
}
