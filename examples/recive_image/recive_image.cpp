#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

class WebSocketClient {
public:
    WebSocketClient(const std::string& uri) : uri_(uri) {

        try {
            // Set logging to be pretty verbose (everything except message payloads)
            client_.set_access_channels(websocketpp::log::alevel::all);
            client_.clear_access_channels(websocketpp::log::alevel::frame_payload);

            // Initialize ASIO
            client_.init_asio();

            // Register our message handler
            client_.set_message_handler(bind(&WebSocketClient::on_message, this, ::_1, ::_2));
        } catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
        }
    }

    void run() {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri_, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        connection_hdl_ = con->get_handle();

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        client_.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        client_.run();
    }

    void stop() {
        client_.close(connection_hdl_, websocketpp::close::status::going_away, "Client shutting down");
        client_.stop();
    }

private:
    typedef websocketpp::client<websocketpp::config::asio_client> client;
    client client_;
    std::string uri_;
    websocketpp::connection_hdl connection_hdl_;

    void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {
        // Decode the received image
        std::vector<uchar> buf(msg->get_payload().begin(), msg->get_payload().end());
        cv::UMat image = cv::imdecode(buf, cv::IMREAD_COLOR).getUMat(cv::ACCESS_READ);

        // Display the image
        cv::imshow("Received Image", image);
        cv::waitKey(1);
    }
};

int main() {
    WebSocketClient client("ws://localhost:9002");
    std::thread client_thread(&client {
        client.run();
    });

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::seconds(50));

    client.stop();
    client_thread.join();
    return 0;
}
