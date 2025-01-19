#include "http_client.hpp"
#include <boost/beast/version.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include <stdexcept>
#include <iostream>

http_client::http_client(const std::string& host, const std::string& port)
    : ssl_ctx_(boost::asio::ssl::context::sslv23_client), stream_(ioc_, ssl_ctx_),
    host_(host) {
    if(!SSL_set_tlsext_host_name(stream_.native_handle(), host.c_str())) {
        unsigned long ssl_err = ::ERR_get_error();
        throw boost::system::system_error(
            boost::system::error_code(static_cast<int>(ssl_err),boost::asio::error::get_ssl_category()));
    }

    boost::asio::ip::tcp::resolver resolver(ioc_);
    boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(host, port);

    boost::beast::get_lowest_layer(stream_).connect(results);
    //boost::asio::connect(stream_.next_layer(), results);
    stream_.handshake(boost::asio::ssl::stream_base::client);
}

http_client::~http_client() {
    boost::beast::error_code ec;
    stream_.shutdown(ec);
    if(ec && ec != boost::beast::errc::not_connected)
        std::cerr << "Warning: Error during SSL shutdown: " << ec.message() << std::endl;
}

nlohmann::json http_client::get(const std::string& target) {
    boost::beast::http::request<boost::beast::http::string_body> req {boost::beast::http::verb::get, target, 11};
    req.set(boost::beast::http::field::host, host_);
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    boost::beast::http::write(stream_, req);

    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> res;
    boost::beast::http::read(stream_, buffer, res);

    if(res.result() != boost::beast::http::status::ok) {
        std::cerr << "Get request failed. HTTP response code: " << res.result_int() << std::endl;
        return nlohmann::json{};
    }
    try{
        return nlohmann::json::parse(res.body());
    }catch(const nlohmann::json::exception& e){
        std::cerr << "Failed ti parce JSON response: " << e.what() << std::endl;
        return nlohmann::json{};
    }
}

nlohmann::json http_client::post(const std::string& target, const std::string& body) {
    boost::beast::http::request<boost::beast::http::string_body> req {boost::beast::http::verb::post, target, 11};
    req.set(boost::beast::http::field::host, host_);
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(boost::beast::http::field::content_type, "application/x-www-form-urlencoded");
    req.body() = body;
    req.prepare_payload();

    boost::beast::http::write(stream_, req);

    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::string_body> res;
    boost::beast::http::read(stream_, buffer, res);

    if(res.result() != boost::beast::http::status::ok) {
        std::cerr << "Get request failed. HTTP response code: " << res.result_int() << std::endl;
        return nlohmann::json{};
    }
    try{
        return nlohmann::json::parse(res.body());
    }catch(const nlohmann::json::exception& e){
        std::cerr << "Failed ti parce JSON response: " << e.what() << std::endl;
        return nlohmann::json{};
    }
}
void http_client::addRequest(const std::string& name, boost::beast::http::verb method,
    const std::string& target){
    request_store_[name] = {method, target};
}
nlohmann::json http_client::executeRequest(const std::string& name, const std::string& path,
    const std::string& body){
    if (request_store_.find(name) == request_store_.end()) {
        std::cerr << "Request with name '" << name << "' not found." << std::endl;
        return nlohmann::json{};
    }
    const auto& request_info = request_store_.at(name);
    std::string get_query = request_info.target + path;
    if(request_info.method == boost::beast::http::verb::get)
        return get(get_query);
    else if (request_info.method == boost::beast::http::verb::post){
        return post(get_query,body);
    } else {
        std::cerr << "Type query " << request_info.method << " not implemented" << std::endl;
        return nlohmann::json{};
    }
        
}