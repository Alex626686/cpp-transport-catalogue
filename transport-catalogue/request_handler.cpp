#include "request_handler.h"





const tc::TransportCatalogue& RequestHandler::GetCatalogue() const{
	return catalogue_;
}

const TransportRouter& RequestHandler::GetRouter() const{
	return router_;
}
