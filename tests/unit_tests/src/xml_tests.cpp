#include "utest.h"
#include "fst/stream.h"
#include "fst/xml.h"
#include "fst/vector.h"
#include "fst/string.h"
#include "fst/file.h"

namespace
{
    TEST_CASE("fst::xml", "[core]")
    {

        fst::string_view xml_content = R"(<?xml version="1.0" encoding="utf-8"?>
<note>
<!-- Alex -->
<to>Tove</to>
<from>Jani</from>
<heading>Reminder</heading>
<body>Don't forget me this weekend!</body>
<numbers>
    <value>3</value>
    <value>2</value>
    <value>1</value>
</numbers>
<script>
   <![CDATA[
      <message> Welcome to TutorialsPoint </message>
   ]]>
</script>
</note>)";

        fst::vector<char> buffer;
        buffer.resize(xml_content.size() + 1);

        fst::memcpy(buffer.data(), xml_content.data(), xml_content.size() + 1);

        fst::xml_document doc;
        fst::error_result er = doc.parse(buffer.data());

        REQUIRE(!er);
        if (er) { return; }

        //fst::xml_print(fst::cout, doc);

        fst::xml_node* root_node = doc.first_node("note");
        REQUIRE(root_node);

        fst::xml_node* node = root_node->first_node("to");
        REQUIRE(node);
        //fst::string_view value(node->value(), node->value_size());
        //fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", node->value());

        node = root_node->first_node("from");
        REQUIRE(node);
        //fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", node->value());
        //fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", value);

        node = node->next_sibling();
        REQUIRE(node);
        //value = fst::string_view(node->value(), node->value_size());
        //fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", node->value());

        node = root_node->first_node("numbers");
        REQUIRE(node);

        //for (fst::xml_node* it = node->first_node(); it; it = it->next_sibling())
        //{
        //    //value = fst::string_view(it->value(), it->value_size());
        //    fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", it->value());
        //}
    }

    struct banana
    {
        fst::string name;
        int a, b;

        friend inline fst::xml_stream& operator<<(fst::xml_stream& stream, const banana& b) noexcept
        {
            auto& doc = stream.document();
            fst::xml_node* node = doc.create_node("banana");
            node->append_node(doc.create_node("name", b.name));
            node->append_node(doc.create_node("a", b.a));
            node->append_node(doc.create_node("b", b.b));
            stream.node()->append_node(node);

            return stream;
        }
    };

    //
    //
    //
    TEST_CASE("fst::xml::stream")
    {
        fst::xml_document doc;
        doc.append_node(doc.create_declaration_node());

        fst::xml_node* root = doc.create_node("root");
        doc.append_node(root);

        //fst::xml_stream stream(doc, root);

        //banana a = { "Alex", 32, 45 };
        //stream << a;

        //fst::xml_print(fst::cout, doc);
    }

    //
    //
    //
    TEST_CASE("fst::xml::output")
    {

        fst::xml_document doc;

        doc.append_node(doc.create_declaration_node());

        //version="1.0" encoding="UTF-8"

        fst::xml_node* node = doc.allocate_node(fst::xml_node_type::element, "a");

        fst::xml_node* node2 = doc.allocate_node(fst::xml_node_type::element, "b", "Gggoogle");
        node->append_node(node2);

        fst::xml_node* jnode = doc.create_node("John", 23);

        jnode->append_attribute(doc.create_attribute("Alex", "kk"));
        jnode->append_attribute(doc.create_attribute("Alex2", 35));
        node->append_node(jnode);
        node->append_node(doc.create_node("John2", 23.4f));
        node->append_node(doc.create_node("John4", "Peter"));

        fst::xml_node* array_node = doc.create_node("array");
        array_node->append_node(doc.create_node("item", 1));
        array_node->append_node(doc.create_node("item", 2));
        array_node->append_node(doc.create_node("item", 3));
        node->append_node(array_node);

        doc.append_node(node);

        //fst::print(node->value());

        //fst::xml_print(fst::cout, doc);

        //
        {
            const char* fpath = FST_TEST_OUTPUT_RESOURCES_DIRECTORY "/banana.xml";
            fst::file file;
            REQUIRE(file.open(fpath, fst::open_mode::write | fst::open_mode::create_always));

            fst::output_stream<char> stream = fst::file_stream(file);
            fst::xml_print(stream, doc);
            REQUIRE(file.close());
        }

        //
        {
            //fst::string str;
            //fst::xml_print_output(fst::back_insert_iterator(str), doc, 0);
            //fst::print(str);
        }

        //
        {
            //fst::string str;
            //fst::output_stream<char> stream = fst::string_stream(str);
            //fst::xml_print(stream, doc);
            //fst::print(str);
        }
    }

} // namespace
