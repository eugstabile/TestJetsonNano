#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <direct.h>  
#include <windows.h>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void SaveFrame(void *data, uint32_t length, std::string file);

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
        
        std::vector<uchar> buf(msg->get_payload().begin(), msg->get_payload().end());

        size_t length = buf.size();
        void* data = static_cast<void*>(buf.data());
        std::string file = "frame.raw";

        // Crear y abrir el archivo
        HANDLE hFile = CreateFile(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Unable to save frame, cannot open file " << file << std::endl;
            return;
        }
        
        // Escribir los datos en el archivo
        DWORD writtenBytes;
        BOOL result = WriteFile(hFile, data, length, &writtenBytes, NULL);
        if (!result) {
            std::cerr << "Error writing to file " << file << std::endl;
        } else if (writtenBytes != length) {
            std::cerr << "Warning: " << writtenBytes << " out of " << length << " were written to file " << file << std::endl;
        }

        // Cerrar el archivo
        CloseHandle(hFile);
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
    //std::this_thread::sleep_for(std::chrono::seconds(1));

    client.stop();
    client_thread.join();
    
    return 0;

}
