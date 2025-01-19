#ifndef HTTP_CLIENT
#define HTTP_CLIENT

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <nlohmann/json.hpp>
#include <string>

class http_client
{
public:
    struct RequestInfo{
        boost::beast::http::verb method;
        std::string target;
    };
    http_client(const std::string& host, const std::string& port);
    ~http_client();

    nlohmann::json get(const std::string& target);
    nlohmann::json post(const std::string& target, const std::string& body);
    // Add Request to storage
    void addRequest(const std::string& name, boost::beast::http::verb method, const std::string& target);
    // Use Request from storage
    nlohmann::json executeRequest(const std::string& name, const std::string& path, const std::string& body);
private:
    boost::asio::io_context ioc_;
    boost::asio::ssl::context ssl_ctx_;
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    std::string host_;
    std::unordered_map<std::string, RequestInfo> request_store_;
};

#endif
