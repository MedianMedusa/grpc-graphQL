#include <string>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include <grpcpp/grpcpp.h>
#include "phones.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using phones::Phones;
using phones::Request;
using phones::Response;

class Service : public Phones::Service {
	std::string phones[2] = {"1234", "0123"};
	std::string codes[2] = {"code1", "code2"};

	Status sendRequest(
		ServerContext *context,
		const Request *request,
		Response *response
		) 
	override {
		//string phone = request->phone();
		//string code = request->code();
						
		std::string *s = std::find(std::begin(phones), std::end(phones), request->phone());

		//phone doesnt exist
		if (s == std::end(phones)) {
			response->set_found(false);
			return Status::OK;
		}

		//phone exists but codes dont match
		if (codes[std::distance(phones, s)] != request->code()) {
			response->set_found(true);
			response->set_approved(false);
			return Status::OK;
		}

		response->set_found(true);
		response->set_approved(true);
		return Status::OK;
	}
};		

int main (int argc, char* argv[]) {
	Service service;
	ServerBuilder builder;

	builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "server started on 50051\n";

	server->Wait();
	
	return 0;
}
