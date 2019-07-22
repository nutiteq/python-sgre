#include <cstdint>
#include <vector>
#include <iostream>
#include <initializer_list>
#include <string>
#include <codecvt>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "Base.h"
#include "Graph.h"
#include "GraphBuilder.h"
#include "Rule.h"
#include "Query.h"
#include "Instruction.h"
#include "Result.h"
#include "RouteFinder.h"

namespace {
    std::initializer_list<std::pair<const char*, carto::sgre::Instruction::Type>> instructionTypeTable = {
        { "HEAD_ON", carto::sgre::Instruction::Type::HEAD_ON },
        { "GO_STRAIGHT", carto::sgre::Instruction::Type::GO_STRAIGHT },
        { "GO_UP", carto::sgre::Instruction::Type::GO_UP },
        { "GO_DOWN", carto::sgre::Instruction::Type::GO_DOWN },
        { "TURN_RIGHT", carto::sgre::Instruction::Type::TURN_RIGHT },
        { "TURN_LEFT", carto::sgre::Instruction::Type::TURN_LEFT },
        { "WAIT", carto::sgre::Instruction::Type::WAIT },
        { "REACHED_YOUR_DESTINATION", carto::sgre::Instruction::Type::REACHED_YOUR_DESTINATION }
    };

    std::initializer_list<std::pair<const char*, carto::sgre::Result::Status>> resultStatusTable = {
        { "FAILED", carto::sgre::Result::Status::FAILED },
        { "SUCCESS", carto::sgre::Result::Status::SUCCESS }
    };

    std::string serializeJSON(const picojson::value& value) {
        return value.serialize();
    }

    picojson::value parseJSON(const std::string& json) {
        picojson::value value;
        std::string err = picojson::parse(value, json);
        if (!err.empty()) {
            throw std::runtime_error(err);
        }
        return value;
    }

    picojson::value py2picojson(const boost::python::object& obj) {
        boost::python::extract<boost::python::dict> dictObj(obj);
        if (dictObj.check()) {
            boost::python::dict dict = dictObj;
            boost::python::list keys = dict.keys();

            picojson::object obj;
            for (int i = 0; i < len(keys); i++) {
                boost::python::extract<std::string> extractedKey(keys[i]);
                if (!extractedKey.check()) {
                    throw std::runtime_error("expecting string key");
                }
                std::string key = extractedKey;
                picojson::value val = py2picojson(dict[key]);
                obj[key] = val;
            }
            return picojson::value(obj);
        }

        boost::python::extract<boost::python::list> listObj(obj);
        if (listObj.check()) {
            boost::python::list list = listObj;

            picojson::array arr;
            for (int i = 0; i < len(list); i++) {
                picojson::value val = py2picojson(list[i]);
                arr.push_back(val);
            }
            return picojson::value(arr);
        }

        std::string className = boost::python::extract<std::string>(obj.attr("__class__").attr("__name__"));
        if (className == "NoneType") {
            return picojson::value();
        }
        if (className == "bool") {
            bool val = boost::python::extract<bool>(obj);
            return picojson::value(val);
        }
        if (className == "int" || className == "long") {
            std::int64_t val = boost::python::extract<std::int64_t>(obj);
            return picojson::value(val);
        }
        boost::python::extract<std::string> stringObj(obj);
        if (stringObj.check()) {
            std::string str = stringObj;
            return picojson::value(str);
        }
        boost::python::extract<std::wstring> wstringObj(obj);
        if (wstringObj.check()) {
            std::wstring wstr = wstringObj;
            return picojson::value(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wstr));
        }
        boost::python::extract<double> doubleObj(obj);
        if (doubleObj.check()) {
            double val = doubleObj;
            return picojson::value(val);
        }
        throw std::runtime_error("failed to convert python object");
    }

    boost::python::object picojson2py(const picojson::value& val) {
        if (val.is<picojson::object>()) {
            boost::python::dict dict;
            for (auto item : val.get<picojson::object>()) {
                dict[item.first] = picojson2py(item.second);
            }
            return dict;
        }

        if (val.is<picojson::array>()) {
            boost::python::list list;
            for (auto item : val.get<picojson::array>()) {
                list.append(picojson2py(item));
            }
            return list;
        }

        if (val.is<std::string>()) {
            return boost::python::object(val.get<std::string>());
        }
        if (val.is<std::int64_t>()) {
            return boost::python::object(val.get<std::int64_t>());
        }
        if (val.is<double>()) {
            return boost::python::object(val.get<double>());
        }
        if (val.is<bool>()) {
            return boost::python::object(val.get<bool>());
        }
        return boost::python::object();
    }

    template <std::size_t N>
    double getPointComponent(const carto::sgre::Point& point) {
        return point(N);
    }

    template <std::size_t N>
    void setPointComponent(carto::sgre::Point& point, double val) {
        point(N) = val;
    }

    template <std::size_t N>
    carto::sgre::Point getQueryPoint(const carto::sgre::Query& query) {
        return query.getPos(N);
    }

    template <std::size_t N>
    void setQueryPoint(carto::sgre::Query& query, const carto::sgre::Point& pos) {
        query.setPos(N, pos);
    }

    template <std::size_t N>
    boost::python::dict getQueryFilter(const carto::sgre::Query& query) {
        boost::python::dict dict;
        for (auto item : query.getFilter(N)) {
            dict[item.first] = picojson2py(item.second);
        }
        return dict;
    }

    template <std::size_t N>
    void setQueryFilter(carto::sgre::Query& query, const boost::python::dict& dict) {
        picojson::object filter;
        boost::python::list keys = dict.keys();  
        for (int i = 0; i < len(keys); i++) {  
            boost::python::extract<std::string> extractedKey(keys[i]);  
            if (!extractedKey.check()){  
                throw std::runtime_error("expecting string key");
            }  
            std::string key = extractedKey;  
            picojson::value val = py2picojson(dict[key]);
            filter[key] = val;  
        }  
        query.setFilter(N, filter);
    }

    std::string getInstructionTag(const carto::sgre::Instruction& instr) {
        return serializeJSON(instr.getTag());
    }

    std::vector<carto::sgre::Point> getResultGeometry(const carto::sgre::Result& result) {
        return result.getGeometry();
    }

    std::vector<carto::sgre::Instruction> getResultInstructions(const carto::sgre::Result& result) {
        return result.getInstructions();
    }

    std::shared_ptr<carto::sgre::RouteFinder> createRouteFinder(const std::string& geoJSON, const std::string& config, const std::string& profile) {
        picojson::value geoJSONDef = parseJSON(geoJSON);
        picojson::value configDef = parseJSON(config);
        carto::sgre::RuleList ruleList;
        if (configDef.contains("rules")) {
            ruleList = carto::sgre::RuleList::parse(configDef.get("rules"));
        }
        ruleList.filter(profile);
        carto::sgre::GraphBuilder graphBuilder(ruleList);
        graphBuilder.importGeoJSON(geoJSONDef);
        return carto::sgre::RouteFinder::create(graphBuilder.build(), configDef);
    }
}

namespace std {
    std::ostream& operator << (std::ostream& os, const std::vector<carto::sgre::Point>& points) {
        os << "[";
        for (std::size_t i = 0; i < points.size(); i++) {
            os << (i > 0 ? "," : "") << points[i];
        }
        os << "]";
        return os;
    }
}

namespace carto { namespace sgre {
    bool operator == (const carto::sgre::Instruction& instr1, const carto::sgre::Instruction& instr2) {
        return instr1.serialize() == instr2.serialize();
    }

    std::ostream& operator << (std::ostream& os, const Query& query) {
        os << "Query(";
        os << "origin=" << query.getPos(0) << ",destination=" << query.getPos(1) << ",";
        os << "origin_filter=" << picojson::value(query.getFilter(0)).serialize() << ",destination_filter=" << picojson::value(query.getFilter(1)).serialize();
        os << ")";
        return os;
    }

    std::ostream& operator << (std::ostream& os, const Instruction& instr) {
        os << "Instruction(";
        for (const auto& nameValue : instructionTypeTable) {
            if (nameValue.second == instr.getType()) {
                os << "type=" << nameValue.first << ",";
            }
        }
        os << "tag=" << instr.getTag().serialize() << ",distance=" << instr.getDistance() << ",time=" << instr.getTime() << ",geometryindex=" << instr.getGeometryIndex();
        os << ")";
        return os;
    }

    std::ostream& operator << (std::ostream& os, const std::vector<Instruction>& instructions) {
        os << "[";
        for (std::size_t i = 0; i < instructions.size(); i++) {
            os << (i > 0 ? "," : "") << instructions[i];
        }
        os << "]";
        return os;
    }

    std::ostream& operator << (std::ostream& os, const Result& result) {
        os << "Result(";
        for (const auto& nameValue : resultStatusTable) {
            if (nameValue.second == result.getStatus()) {
                os << "status=" << nameValue.first << ",";
            }
        }
        os << "instructions=" << result.getInstructions() << ",geometry=" << result.getGeometry() << ",totaldistance=" << result.getTotalDistance() << ",totaltime=" << result.getTotalTime();
        os << ")";
        return os;
    }

    std::ostream& operator << (std::ostream& os, const RouteFinder& routeFinder) {
        os << "RouteFinder(";
        const RouteFinder::RouteOptions& routeOptions = routeFinder.getRouteOptions();
        os << "pathstraightening=" << routeOptions.pathStraightening << ",tesselationdistance=" << routeOptions.tesselationDistance << ",zsensitivity=" << routeOptions.zSensitivity << ",min_turnangle=" << routeOptions.minTurnAngle << ",min_updownangle=" << routeOptions.minUpDownAngle;
        os << ")";
        return os;
    }

    std::ostream& operator << (std::ostream& os, const std::shared_ptr<RouteFinder>& routeFinder) {
        if (!routeFinder) {
            os << "RouteFinder()";
        } else {
            os << *routeFinder;
        }
        return os;
    }
} }

BOOST_PYTHON_MODULE(sgre) {
    using namespace boost::python;

    class_<carto::sgre::Point>("Point", init<double, double, double>((boost::python::arg("x"), boost::python::arg("y"), boost::python::arg("z")=0.0)))
       .def(self_ns::str(self_ns::self))
       .add_property("x", &getPointComponent<0>, &setPointComponent<0>)
       .add_property("y", &getPointComponent<1>, &setPointComponent<1>)
       .add_property("z", &getPointComponent<2>, &setPointComponent<2>);

    class_<std::vector<carto::sgre::Point>>("PointList")
       .def(self_ns::str(self_ns::self))
       .def(vector_indexing_suite<std::vector<carto::sgre::Point>>());

    class_<carto::sgre::Query>("Query", init<carto::sgre::Point, carto::sgre::Point>())
       .def(self_ns::str(self_ns::self))
       .add_property("origin", &getQueryPoint<0>, &setQueryPoint<0>)
       .add_property("destination", &getQueryPoint<1>, &setQueryPoint<1>)
       .add_property("origin_filter", &getQueryFilter<0>, &setQueryFilter<0>)
       .add_property("destination_filter", &getQueryFilter<1>, &setQueryFilter<1>);

    enum_<carto::sgre::Instruction::Type> instructionTypeEnum("InstructionType");
    for (const auto& nameValue : instructionTypeTable) {
       instructionTypeEnum.value(nameValue.first, nameValue.second);
    }

    class_<carto::sgre::Instruction>("Instruction", no_init)
       .def(self_ns::str(self_ns::self))
       .add_property("type", &carto::sgre::Instruction::getType)
       .add_property("tag", &getInstructionTag)
       .add_property("distance", &carto::sgre::Instruction::getDistance)
       .add_property("time", &carto::sgre::Instruction::getTime)
       .add_property("geometryindex", &carto::sgre::Instruction::getGeometryIndex);

    class_<std::vector<carto::sgre::Instruction>>("InstructionList")
       .def(self_ns::str(self_ns::self))
       .def(vector_indexing_suite<std::vector<carto::sgre::Instruction>>());

    enum_<carto::sgre::Result::Status> resultStatusEnum("ResultStatus");
    for (const auto& nameValue : resultStatusTable) {
       resultStatusEnum.value(nameValue.first, nameValue.second);
    }

    class_<carto::sgre::Result>("Result", no_init)
       .def(self_ns::str(self_ns::self))
       .add_property("status", &carto::sgre::Result::getStatus)
       .add_property("instructions", &getResultInstructions)
       .add_property("geometry", &getResultGeometry)
       .add_property("totaldistance", &carto::sgre::Result::getTotalDistance)
       .add_property("totaltime", &carto::sgre::Result::getTotalTime);

    class_<carto::sgre::RouteFinder, std::shared_ptr<carto::sgre::RouteFinder>>("RouteFinder", no_init)
       .def(self_ns::str(self_ns::self))
       .def("find", &carto::sgre::RouteFinder::find);

    def("create_routefinder", &createRouteFinder);
}
