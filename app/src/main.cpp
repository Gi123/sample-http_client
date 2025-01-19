#include "http_client.hpp"
#include <iostream>

void print_response(const nlohmann::json& response){
    if(!response.empty()){
        std::cout << "Response 1 " << response.dump(4) << std::endl;
    } else{
        std::cerr << "Get returned empty JSON" << std::endl;
    }
}

int main() {
    try
    {
        std::string host = "www.example.com";
        std::string port = "443";
        http_client client(host,port);
        client.addRequest("GetToDO", boost::beast::http::verb::get, "/todos");
        client.addRequest("PostData", boost::beast::http::verb::post, "/posts");
        client.addRequest("PutData", boost::beast::http::verb::put, "/puts");
        nlohmann::json empty_json = client.executeRequest("UnknownQuery", "/", "");
        print_response(empty_json);
        nlohmann::json response1 = client.get("/todos");
        print_response(response1);
        nlohmann::json response2 = client.executeRequest("GetToDO", "/", "");
        print_response(response2);
        nlohmann::json post_responce = {
            {"title", "foo"},
            {"body", "bar"},
            {"userId", 1}
        };
        nlohmann::json response3 = client.post("/posts",post_responce.dump());
        print_response(response3);
        nlohmann::json response4 = client.executeRequest("PostData", "/", post_responce.dump());
        print_response(response4);
        nlohmann::json response5 = client.executeRequest("PutData", "/", post_responce.dump());
        print_response(response5);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}