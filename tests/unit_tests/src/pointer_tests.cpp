#include "utest.h"
#include "fst/pointer.h"
#include "fst/array.h"

#include "fst/stream.h"
#include "fst/xml.h"
#include "fst/vector.h"
#include "fst/string.h"

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
       if(er)
           {
           return;
       }

        fst::xml_node* root_node = doc.first_node("note");
        REQUIRE(root_node);

        fst::xml_node* node = root_node->first_node("to");
        REQUIRE(node);
        //fst::string_view value(node->value(), node->value_size());
        fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", node->value());

        node = root_node->first_node("from");
        REQUIRE(node);
        fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", node->value());
        //fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", value);

        node = node->next_sibling();
        REQUIRE(node);
        //value = fst::string_view(node->value(), node->value_size());
        fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", node->value());

        node = root_node->first_node("numbers");
        REQUIRE(node);

        for (fst::xml_node* it = node->first_node(); it; it = it->next_sibling())
        {
            //value = fst::string_view(it->value(), it->value_size());
            fst::print("DSLKDJSKLDJSKJDKLSJDKLJDS", it->value());
        }
    }

    TEST_CASE("fst::unique_ptr", "[pointer]")
    {
        fst::unique_ptr<int> ptr = fst::unique_ptr<int>::make(32);
        REQUIRE(ptr);
        REQUIRE(*ptr == 32);

        *ptr = 34;
        REQUIRE(*ptr == 34);

        ptr.reset();

        REQUIRE(!ptr);
    }
} // namespace
