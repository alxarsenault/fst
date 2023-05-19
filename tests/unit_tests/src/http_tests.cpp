#include "utest.h"
#include "fst/http.h"
#include "fst/unicode.h"

#if 0//__FST_WINDOWS__
namespace
{
    TEST_CASE("fst::http", "[network]")
    {
        /*{
            fst::http_url url = fst::http_url("johnbnfd");
            fst::http_response resp = url.send();

            REQUIRE(!resp);
            fst::print(resp.error());
        }*/
        {
            fst::string_view furl = "https://dummyjson.com/products/search?q=phone";
            fst::http_url url = fst::http_url("https://dummyjson.com/products/search").with_parameter({ "q", "phone" });

            fst::http_request req(fst::move(url));
            fst::http_response resp = req.send();

            REQUIRE(resp.has_data());
            fst::string_view data = resp.as_string();
            fst::string_view rr
                = R"({"products":[{"id":1,"title":"iPhone 9","description":"An apple mobile which is nothing like apple","price":549,"discountPercentage":12.96,"rating":4.69,"stock":94,"brand":"Apple","category":"smartphones","thumbnail":"https://i.dummyjson.com/data/products/1/thumbnail.jpg","images":["https://i.dummyjson.com/data/products/1/1.jpg","https://i.dummyjson.com/data/products/1/2.jpg","https://i.dummyjson.com/data/products/1/3.jpg","https://i.dummyjson.com/data/products/1/4.jpg","https://i.dummyjson.com/data/products/1/thumbnail.jpg"]},{"id":2,"title":"iPhone X","description":"SIM-Free, Model A19211 6.5-inch Super Retina HD display with OLED technology A12 Bionic chip with ...","price":899,"discountPercentage":17.94,"rating":4.44,"stock":34,"brand":"Apple","category":"smartphones","thumbnail":"https://i.dummyjson.com/data/products/2/thumbnail.jpg","images":["https://i.dummyjson.com/data/products/2/1.jpg","https://i.dummyjson.com/data/products/2/2.jpg","https://i.dummyjson.com/data/products/2/3.jpg","https://i.dummyjson.com/data/products/2/thumbnail.jpg"]},{"id":71,"title":"Women Shoulder Bags","description":"LouisWill Women Shoulder Bags Long Clutches Cross Body Bags Phone Bags PU Leather Hand Bags Large Capacity Card Holders Zipper Coin Purses Fashion Crossbody Bags for Girls Ladies","price":46,"discountPercentage":14.65,"rating":4.71,"stock":17,"brand":"LouisWill","category":"womens-bags","thumbnail":"https://i.dummyjson.com/data/products/71/thumbnail.jpg","images":["https://i.dummyjson.com/data/products/71/1.jpg","https://i.dummyjson.com/data/products/71/2.jpg","https://i.dummyjson.com/data/products/71/3.webp","https://i.dummyjson.com/data/products/71/thumbnail.jpg"]},{"id":86,"title":"Bluetooth Aux","description":"Bluetooth Aux Bluetooth Car Aux Car Bluetooth Transmitter Aux Audio Receiver Handfree Car Bluetooth Music Receiver Universal 3.5mm Streaming A2DP Wireless Auto AUX Audio Adapter With Mic For Phone MP3","price":25,"discountPercentage":10.56,"rating":4.57,"stock":22,"brand":"Car Aux","category":"automotive","thumbnail":"https://i.dummyjson.com/data/products/86/thumbnail.jpg","images":["https://i.dummyjson.com/data/products/86/1.jpg","https://i.dummyjson.com/data/products/86/2.webp","https://i.dummyjson.com/data/products/86/3.jpg","https://i.dummyjson.com/data/products/86/4.jpg","https://i.dummyjson.com/data/products/86/thumbnail.jpg"]}],"total":4,"skip":0,"limit":4})";

            REQUIRE(data == rr);
        }

        {
            fst::http_url url("www.example.com");
            fst::http_response resp = url.send();

            REQUIRE(resp);
            REQUIRE(resp.status());
            REQUIRE(resp.status().is_success());
            REQUIRE(resp.has_data());

            //for (const auto& h : resp.get_headers())
            //{
            //fst::print("NAME", h.name, "VALUE", h.value);
            //}
        }

        {
            fst::string_view furl = "https://dummyjson.com/products/search?q=phone";

            fst::http_url url = fst::http_url("https://dummyjson.com/products/search").with_parameter({ "q", "phone" });
            REQUIRE(url.get_scheme() == "https://");
            REQUIRE(url.get_domain() == "dummyjson.com");

            REQUIRE(url.get_route() == "/products/search");
            REQUIRE(url.get_route(true) == "/products/search?q=phone");
            REQUIRE(url.get_parameters_string() == "?q=phone");
            REQUIRE(url.get_string(true, true) == furl);

            fst::http_response resp = url.send();

            REQUIRE(resp.has_data());
            fst::string_view data = resp.as_string();
            fst::string_view rr
                = R"({"products":[{"id":1,"title":"iPhone 9","description":"An apple mobile which is nothing like apple","price":549,"discountPercentage":12.96,"rating":4.69,"stock":94,"brand":"Apple","category":"smartphones","thumbnail":"https://i.dummyjson.com/data/products/1/thumbnail.jpg","images":["https://i.dummyjson.com/data/products/1/1.jpg","https://i.dummyjson.com/data/products/1/2.jpg","https://i.dummyjson.com/data/products/1/3.jpg","https://i.dummyjson.com/data/products/1/4.jpg","https://i.dummyjson.com/data/products/1/thumbnail.jpg"]},{"id":2,"title":"iPhone X","description":"SIM-Free, Model A19211 6.5-inch Super Retina HD display with OLED technology A12 Bionic chip with ...","price":899,"discountPercentage":17.94,"rating":4.44,"stock":34,"brand":"Apple","category":"smartphones","thumbnail":"https://i.dummyjson.com/data/products/2/thumbnail.jpg","images":["https://i.dummyjson.com/data/products/2/1.jpg","https://i.dummyjson.com/data/products/2/2.jpg","https://i.dummyjson.com/data/products/2/3.jpg","https://i.dummyjson.com/data/products/2/thumbnail.jpg"]},{"id":71,"title":"Women Shoulder Bags","description":"LouisWill Women Shoulder Bags Long Clutches Cross Body Bags Phone Bags PU Leather Hand Bags Large Capacity Card Holders Zipper Coin Purses Fashion Crossbody Bags for Girls Ladies","price":46,"discountPercentage":14.65,"rating":4.71,"stock":17,"brand":"LouisWill","category":"womens-bags","thumbnail":"https://i.dummyjson.com/data/products/71/thumbnail.jpg","images":["https://i.dummyjson.com/data/products/71/1.jpg","https://i.dummyjson.com/data/products/71/2.jpg","https://i.dummyjson.com/data/products/71/3.webp","https://i.dummyjson.com/data/products/71/thumbnail.jpg"]},{"id":86,"title":"Bluetooth Aux","description":"Bluetooth Aux Bluetooth Car Aux Car Bluetooth Transmitter Aux Audio Receiver Handfree Car Bluetooth Music Receiver Universal 3.5mm Streaming A2DP Wireless Auto AUX Audio Adapter With Mic For Phone MP3","price":25,"discountPercentage":10.56,"rating":4.57,"stock":22,"brand":"Car Aux","category":"automotive","thumbnail":"https://i.dummyjson.com/data/products/86/thumbnail.jpg","images":["https://i.dummyjson.com/data/products/86/1.jpg","https://i.dummyjson.com/data/products/86/2.webp","https://i.dummyjson.com/data/products/86/3.jpg","https://i.dummyjson.com/data/products/86/4.jpg","https://i.dummyjson.com/data/products/86/thumbnail.jpg"]}],"total":4,"skip":0,"limit":4})";

            REQUIRE(data == rr);
        }

        //
        {

            fst::http_url url("www.google.com/banana");

            REQUIRE(url.get_route() == "/banana");

            REQUIRE(url.get_domain() == "www.google.com");
            REQUIRE_FALSE(url.has_scheme());
            REQUIRE_FALSE(url.has_parameters());

            url.with_scheme(fst::http_scheme::http);
            //fst::print(url.get_scheme());
            REQUIRE(url.has_scheme());

            REQUIRE(url.get_scheme() == "http://");

            url.with_parameter({ "peter", "123" });
            REQUIRE(url.has_parameters());

            const fst::http_parameter* peter_param = url.get_parameter("peter");
            REQUIRE(peter_param);
            REQUIRE_EQ(peter_param->value, "123");
        }
    }

    TEST_CASE("fst::request")
    {
        fst::http_url url = fst::http_url("https://dummyjson.com/products/add");

        fst::http_request req(fst::move(url));
        req.set_method(fst::http_method::POST);
        req.add_header({ "Content-Type", "application/json" });
        req.set_body("{\"title\":\"john\"}");

        fst::http_response resp = req.send();

        REQUIRE(resp.has_data());
        fst::string_view data = resp.as_string();
        fst::print(data);
        const auto& headers = resp.get_headers();

        for (const auto& c : headers)
        {
            fst::print("================", c.name, "VALUE", c.value);
        }
    }
} // namespace
#endif //
