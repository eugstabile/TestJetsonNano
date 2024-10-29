#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

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
            client_.set_message_handler(bind(&WebSocketClient::on_message, this, &client_, ::_1, ::_2));
            client_.set_open_handler(bind(&WebSocketClient::on_open, this, &client_, ::_1));
        } catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
        }
    }

    void run() {

        websocketpp::lib::error_code ec;
        client::connection_ptr con = client_.get_connection(uri_, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return;
        }

        client_.connect(con);
        connection_hdl_ = con->get_handle();

        cv::namedWindow("Received Image", cv::WINDOW_AUTOSIZE);
        client_.run();

    }

    void stop() {
        client_.close(connection_hdl_, websocketpp::close::status::going_away, "Client shutting down");
        client_.stop();
    }
    
    void send() {

        std::string mensaje = "grabFrame";
        websocketpp::lib::error_code ec;
        client_.send(connection_hdl_, mensaje, websocketpp::frame::opcode::text, ec);

        if (ec) {

            std::cout << "Echo failed because: " << ec.message() << std:: endl;

        }
        
    }

private:
    typedef websocketpp::client<websocketpp::config::asio_client> client;
    client client_;
    std::string uri_;
    websocketpp::connection_hdl connection_hdl_;

    void on_message(client* c, websocketpp::connection_hdl hdl, client::message_ptr msg) {

        // Decode the received image
        std::vector<uchar> buf(msg->get_payload().begin(), msg->get_payload().end());

         std::cout << "Primeros valores de la imagen RAW10: ";
        for (int i = 0; i < 10; ++i) {
            std::cout << static_cast<int>(buf[i]) << " ";
        }
    
        cv::Mat image(720, 1280, CV_16UC1, buf.data());
        
        cv::Mat image8bit;
        cv::normalize(image, image8bit, 0, 255, cv::NORM_MINMAX, CV_8UC1);


        if (image8bit.empty()) {
            std::cout << "Image empty." << std::endl;
        } else {
            // Display the image
            cv::imshow("Received Image", image8bit);
            cv::waitKey(1);
        }

        send();

    }
    
    void on_open(client* c, websocketpp::connection_hdl hdl) {
        send();
    }

};

int main() {

    const std::string IP_SERVER = "192.168.1.229";
    std::string ip = "ws://" + IP_SERVER +  ":9002";
    WebSocketClient client(ip);
    std::thread client_thread([&client]()
     {
        client.run();
    });

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::seconds(1000));

    client.stop();
    client_thread.join();
    
    return 0;

}
