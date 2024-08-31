
#include "request_handler.h"
#include "json_reader.h"
int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */

    JsonReader reader(std::cin);
    tc::TransportCatalogue catalogue;
    reader.FillCatalogue(catalogue);
    RequestHandler handler(catalogue, reader.FillRenderSettings(), reader.FillRouterSettings());
    reader.Print(handler);

}