#include <string>
#include <iostream>
#include <stdlib.h>


#include <grpcpp/grpcpp.h>
#include "phones.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using phones::Phones;
using phones::Request;
using phones::Response;

class Client {
public:
	Client(std::shared_ptr<Channel> channel) : stub_(Phones::NewStub(channel)) {}

	void sendRequest(std::string p, std::string c, bool &found, bool &approved) {
		Request request;
		request.set_phone(p);
		request.set_code(c);
		
		Response response;
		ClientContext context;

		Status status = stub_->sendRequest(&context, request, &response);
		if (status.ok()){
			found = response.found();
			approved = response.approved();
		}
		else {
			std::cout << status.error_code() << ": " << status.error_message() << std::endl;
		}
	}

private:
	std::unique_ptr<Phones::Stub> stub_;
		
};



int main (int argc, char* argv[]) {
	Client client(grpc::CreateChannel(
				"0.0.0.0:5000",
				grpc::InsecureChannelCredentials()));
	std::string phone;
	std::string code;
	do{
		std::cout << "enter phone (or q to quit): ";
		std::cin >> phone;
		if (phone == "q") break;
		std::cout << "enter code: ";
		std::cin >> code;
		bool found, approved;
		client.sendRequest(phone, code, found, approved);
		std::cout << "phone found - " << found << ", phone approved - " << approved << std::endl;
		

	} while(true);
	
	return 0;
}
