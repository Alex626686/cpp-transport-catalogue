#include "json_reader.h"
#include "map_renderer.h"

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */

    JsonReader jsr(std::cin);
    tc::TransportCatalogue tk;
    jsr.FillCatalogue(tk);
    jsr.Print(tk);
}