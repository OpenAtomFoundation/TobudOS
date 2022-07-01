#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import sys
import os
import argparse
import glob

from sys import version_info

if version_info.major == 3:
    import importlib
    importlib.reload(sys)
elif version_info.major == 2:
    reload(sys)
    sys.setdefaultencoding("utf-8")

try:
    import simplejson as json
except:
    import json

# {"version":"1.0","properties":[{"id":"light_switch","name":"电灯开关","desc":"控制电灯开灭","required":true,"mode":"rw","define":{"type":"bool","mapping":{"0":"关","1":"开"}}},{"id":"color","name":"颜色","desc":"灯光颜色","mode":"rw","define":{"type":"enum","mapping":{"0":"Red","1":"Green","2":"Blue"}}},{"id":"brightness","name":"颜色","desc":"灯光颜色","mode":"rw","define":{"type":"int","unit":"%","unitDesc":"亮度百分比","min":"0","max":"100"}},{"id":"name","name":"灯位置名称","desc":"灯位置名称：书房、客厅等","mode":"rw","required":true,"define":{"type":"string","min":"0","max":"64"}}]}

class iot_property:
    """Parse iot property json node and get property code.

    Retrieves rows pertaining to the given keys from the Table instance
    represented by table_handle.  String keys will be UTF-8 encoded.

    Args:
    json_node:
        json node include property
    prefix:
        property, action, event.
    Raises:
        ValueError: invalid node
    """
    def __init__(self, json_node, prefix="property", node_define="define"):
        self.prefix = prefix
        self.id = json_node["id"]
        self.type = json_node[node_define]["type"]
        self.default_value = ""
        self.struct_property_list = []
        self.mode = 1 # rw mode
        try:
            # property type dict
            PROPERTY_TYPE_DICT = {
                # key          type_id                            type_value_name
                'bool'      : ["DATA_TEMPLATE_TYPE_BOOL"        , "value_bool"        ], 
                'enum'      : ["DATA_TEMPLATE_TYPE_ENUM"        , "value_enum"        ],
                'float'     : ["DATA_TEMPLATE_TYPE_FLOAT"       , "value_float"       ], 
                'int'       : ["DATA_TEMPLATE_TYPE_INT"         , "value_int"         ],
                'string'    : ["DATA_TEMPLATE_TYPE_STRING"      , "value_string"      ],
                'timestamp' : ["DATA_TEMPLATE_TYPE_TIME"        , "value_time"        ], 
                'struct'    : ["DATA_TEMPLATE_TYPE_STRUCT"      , "value_struct"      ], 
                'stringenum': ["DATA_TEMPLATE_TYPE_STRING_ENUM" , "value_string_enum" ], 
                #'array'     : ["DATA_TEMPLATE_TYPE_ARRAY"       , "value_array"       ], # TODO
            }
            self.type_id = PROPERTY_TYPE_DICT[self.type][0]
            self.type_value_name = PROPERTY_TYPE_DICT[self.type][1]
            # mode: rw or r
            if "mode" in json_node:
                self.mode = 1 if json_node["mode"] == "rw" else 0
            # default value
            if "mapping" in json_node[node_define]:
                self.default_value = next(iter(json_node[node_define]["mapping"]))
            elif "start" in json_node[node_define]:
                self.default_value = json_node[node_define]["start"]
            if self.type_id in ["DATA_TEMPLATE_TYPE_STRUCT"]:
                self.default_value = self.get_property_global_var_name()
            # string
            if self.type_id is "DATA_TEMPLATE_TYPE_STRING":
                self.max_length = json_node[node_define]["max"]
                self.default_value = "\" \""
            if self.type_id is "DATA_TEMPLATE_TYPE_STRING_ENUM":
                self.max_length = 0
                for enum in json_node[node_define]["mapping"]:
                    self.max_length = max(self.max_length, len(enum))
                self.default_value = "\"{}\"".format(next(iter(json_node[node_define]["mapping"])))
                
            # struct
            if "specs" in json_node[node_define]:
                for subnode in json_node[node_define]["specs"]:
                    self.struct_property_list.append(iot_property(subnode, self.prefix + '_' + self.id, "dataType"))
        except KeyError:
            ValueError('{} 字段 数据类型 type={} 取值非法，有效值应为：bool,enum,float,int,,string,timestamp,struct,stringenum'.format(
                self.id, self.type))

    def get_property_enum_index(self):
        return "USR_{}_INDEX_{}".format(self.prefix.upper(), self.id.upper())

    def get_property_global_var_name(self):
        return "sg_usr_{}_{}".format( self.prefix, self.id)

    def create_property_struct_index_enum(self):
        main_code = ""
        main_code += "/*----------------- {} {} index enum start  -------------------*/\n\n".format(self.id, self.prefix)
        main_code += "typedef enum {\n"
        # 构造枚举索引
        for property in self.struct_property_list:
            if self.struct_property_list.index(property) is 0:
                main_code += "    {} = 0,\n".format(property.get_property_enum_index())
            else:
                main_code += "    {},\n".format(property.get_property_enum_index())

        main_code += "} "
        main_code += "Usr{}{}Index;\n\n".format(self.prefix.capitalize(), self.id.capitalize())
        main_code += "/*----------------- {} {} index enum end  -------------------*/\n\n".format(self.id, self.prefix)
        return main_code

    def create_property_struct_definition(self):
        main_code = ""
        if self.type_id != "DATA_TEMPLATE_TYPE_STRUCT":
            return main_code
        # 定义结构体成员数目
        main_code += "#define TOTAL_USR_{}_STRUCT_{}_COUNT {}\n\n".format(self.prefix.upper(), self.id.upper(), len(self.struct_property_list))
        # 定义数据模版结构体
        main_code += "static DataTemplateProperty {}[TOTAL_USR_{}_STRUCT_{}_COUNT];\n\n".format(self.get_property_global_var_name(), self.prefix.upper(), self.id.upper())
        # 定义结构体类型和结构体变量
        main_code += "static void _init_data_template_{}_{}(void)\n".format(self.prefix, self.id)
        main_code += "{\n"
        # 构造结构体数组
        for property in self.struct_property_list:
            static_property_struct_array_index="{}[{}]".format(self.get_property_global_var_name(), property.get_property_enum_index())
            if property.type_id in ["DATA_TEMPLATE_TYPE_STRING", "DATA_TEMPLATE_TYPE_STRING_ENUM"]:
                main_code += "    static char {}[{}+1] = {};\n".format(property.get_property_global_var_name(), property.max_length, property.default_value)
                main_code += "    {}[{}].value.{} = {};\n".format(static_property_struct_array_index, property.type_value_name, property.get_property_global_var_name())
            else:
                main_code += "    {}.value.{} = {};\n".format(static_property_struct_array_index, property.type_value_name, property.default_value)
            main_code += "    {}.key = \"{}\";\n".format(static_property_struct_array_index, property.id)
            main_code += "    {}.type = {};\n".format(static_property_struct_array_index, property.type_id)
            main_code += "    {}.is_rw = {};\n\n".format(static_property_struct_array_index, self.mode)
        # 结尾
        main_code += "}\n\n"
        return main_code

class iot_event:
    def __init__(self, json_node):
        self.event_id = json_node["id"]
        self.event_type = json_node["type"]
        self.event_type_enum= "IOT_DATA_TEMPLATE_EVENT_TYPE_{}".format(self.event_type.upper())
        self.event_properties = []

        for property in json_node["params"]:
            self.event_properties.append(iot_property(property,"event"))

    def get_event_enum_index(self):
        return "USR_EVENT_INDEX_{}".format(self.event_id.upper())
        
    def create_event_params_global_var(self):
        main_code = ""
        main_code += "/**\n * @brief Sample of event {} post params.\n *\n */\n".format(self.event_id)
        main_code += "static const char* sg_usr_event_{}_params  = ".format(self.event_id)
        main_code += "\"{"

        for property in self.event_properties:
            if self.event_properties.index(property) > 0:
                main_code += ","
            if property.type_id in ["DATA_TEMPLATE_TYPE_STRING", "DATA_TEMPLATE_TYPE_STRING_ENUM"]:
                main_code += "\\\"{}\\\":{}".format(property.id, property.default_value.replace('\"','\\\"'))
            else:
                main_code += "\\\"{}\\\":{}".format(property.id, property.default_value)

        main_code += "}\";\n\n"
        return main_code

class iot_action:
    def __init__(self, json_node):
        self.action_id = json_node["id"]

        self.action_input = []
        for input in json_node["input"]:
            self.action_input.append(iot_property(input, 'action_' + self.action_id + '_input'))

        self.action_output = []
        for output in json_node["output"]:
            self.action_output.append(iot_property(output, 'action_' + self.action_id + '_output'))

    def get_action_enum_index(self):
        return "USR_ACTION_INDEX_{}".format(self.action_id.upper())

    def get_action_input_index_enum(self) :
        main_code = ""
        main_code += "/*----------------- action {} input index enum start  -------------------*/\n\n".format(self.action_id)
        main_code += "typedef enum {\n"

        for property in self.action_input:
            if self.action_input.index(property) is 0:
                main_code += "    {} = 0,\n".format(property.get_property_enum_index())
            else:
                main_code += "    {},\n".format(property.get_property_enum_index())
        main_code += "} "
        main_code += "UsrAction{}InputIndex;\n\n".format(self.action_id.capitalize().replace('_', ''))
        main_code += "/*----------------- action {} input index enum end  -------------------*/\n\n".format(self.action_id)
        return main_code

    def create_action_input_init(self):
        static_usr_action_input_array_name = "sg_usr_action_{}_input".format(self.action_id)

        main_code = ""
        main_code += "#define TOTAL_USR_ACTION_{}_INPUT_PARAMS_COUNT {}\n\n".format(self.action_id.upper(), len(self.action_input))
        main_code += "static DataTemplateProperty {}[TOTAL_USR_ACTION_{}_INPUT_PARAMS_COUNT];\n\n".format(static_usr_action_input_array_name, self.action_id.upper())
        main_code += "static void _init_data_template_action_{}_input(void)\n".format(self.action_id)
        main_code += "{\n"

        input_struct = ""
        for property in self.action_input:
            if property.type_id is "DATA_TEMPLATE_TYPE_STRUCT":
                input_struct += property.create_property_struct_definition()
            if property.type_id in ["DATA_TEMPLATE_TYPE_STRING", "DATA_TEMPLATE_TYPE_STRING_ENUM"]:
                main_code += "    static char {}[{}+1]= {};\n".format(property.get_property_global_var_name(), property.max_length, property.default_value)
            array_index = "{}[{}]".format(static_usr_action_input_array_name, property.get_property_enum_index())
            main_code += "    {}.value.{} = {};\n".format(array_index, property.type_value_name, property.default_value)
            main_code += "    {}.key = \"{}\";\n".format(array_index, property.id)
            main_code += "    {}.type = {};\n".format(array_index, property.type_id)
            main_code += "    {}.is_rw = {};\n\n".format(array_index, property.mode)
        main_code += "}\n\n"
        return main_code

    def create_action_reply(self):
        main_code = ""        
        input_struct = ""

        main_code += "/**\n * @brief Sample of action {} reply.\n *\n */\n".format(self.action_id)
        main_code += "static IotDataTemplateActionReply sg_usr_action_{}_reply = ".format(self.action_id)
        main_code += "{\n"
        main_code += "    .code = 0,\n"
        main_code += "    .client_token = {"
        main_code += ".value = \"test_{}\", .value_len = sizeof(\"test_{}\")".format(self.action_id, self.action_id)
        main_code += "},\n"
        main_code += "    .response = \"{"

        for property in self.action_output:
            if self.action_output.index(property) > 0:
                main_code += ","
            if property.type_id == "DATA_TEMPLATE_TYPE_STRUCT":
                input_struct += property.create_property_struct_definition()
            main_code += "\\\"{}\\\":{}".format(property.id, property.default_value)
        main_code += "}\",\n};\n\n"
        # print(main_code)
        return main_code

class iot_struct:
    def __init__(self, model):
        self.properties = []
        self.events = []
        self.actions = []

        if "properties" in model:
            for property in model["properties"]:
                self.properties.append(iot_property(property))

        if "events" in model:
            for event in model["events"]:
                self.events.append(iot_event(event))

        if "actions" in model:
            for action in model["actions"]:
                self.actions.append(iot_action(action))

    def __index_enum_create(self, list, prefix) :
        property_struct_index_enum_str = ""
        action_input_index_enum_str = ""

        main_code = ""
        main_code += "/*----------------- {} index enum start  -------------------*/\n\n".format(prefix)
        main_code += "typedef enum {\n"

        enum_index = ""
        for node in list:
            if prefix is "property":
                enum_index = node.get_property_enum_index()
                if node.type_id is "DATA_TEMPLATE_TYPE_STRUCT":
                    property_struct_index_enum_str += node.create_property_struct_index_enum()
            elif prefix is "action":
                enum_index = node.get_action_enum_index()
                action_input_index_enum_str += node.get_action_input_index_enum()
            elif prefix is "event":
                enum_index = node.get_event_enum_index()

            if list.index(node) is 0:
                main_code += "    {} = 0,\n".format(enum_index)
            else:
                main_code += "    {},\n".format(enum_index)

        main_code += "} "
        main_code += "Usr{}Index;\n\n".format(prefix.capitalize())
        main_code += "/*----------------- {} index enum end  -------------------*/\n\n".format(prefix)

        main_code += property_struct_index_enum_str
        main_code += action_input_index_enum_str
        return main_code

    def __property_data_initializer(self) :
        main_code = ""
        main_code += "// ----------------------------------------------------------------------------\n"
        main_code += "// user property\n"
        main_code += "// ----------------------------------------------------------------------------/\n\n"

        property_code = ""
        property_code += "#define TOTAL_USR_PROPERTY_COUNT {}\n\n".format(len(self.properties))
        property_code += "static DataTemplateProperty sg_usr_data_template_property[TOTAL_USR_PROPERTY_COUNT];\n\n"
        property_code += "static void _init_data_template_property(void)\n"
        property_code += "{\n"
        for property in self.properties:                       
            if property.type_id is "DATA_TEMPLATE_TYPE_STRUCT":
                main_code += property.create_property_struct_definition()
                property_code += "     _init_data_template_property_{}();\n".format(property.id)

            if property.type_id in ["DATA_TEMPLATE_TYPE_STRING", "DATA_TEMPLATE_TYPE_STRING_ENUM"]:
                property_code += "    static char {}[{}+1] = {};\n".format(property.get_property_global_var_name(), property.max_length, property.default_value)

            static_usr_property_array_name = "sg_usr_data_template_property[{}]".format(property.get_property_enum_index())

            if property.type_id is "DATA_TEMPLATE_TYPE_STRUCT":
                property_code += "    {}.value.{}.property = {};\n".format(static_usr_property_array_name, property.type_value_name, property.default_value)
                property_code += "    {}.value.{}.count = TOTAL_USR_PROPERTY_STRUCT_{}_COUNT;\n".format(static_usr_property_array_name, property.type_value_name, property.id.upper())
            elif property.type_id in ["DATA_TEMPLATE_TYPE_STRING", "DATA_TEMPLATE_TYPE_STRING_ENUM"]:
                property_code+= "    {}.value.{} = {};\n".format(static_usr_property_array_name, property.type_value_name, property.get_property_global_var_name())
            else:
                property_code += "    {}.value.{} = {};\n".format(static_usr_property_array_name, property.type_value_name, property.default_value)
            property_code += "    {}.key = \"{}\";\n".format(static_usr_property_array_name, property.id)
            property_code += "    {}.type = {};\n".format(static_usr_property_array_name, property.type_id)
            property_code += "    {}.need_report = {};\n".format(static_usr_property_array_name, 1)
            property_code += "    {}.is_rw = {};\n\n".format(static_usr_property_array_name, property.mode)
        property_code += "}\n\n"

        main_code += property_code
        return main_code

    def __event_data_initializer(self) :
        main_code = ""
        main_code += "// ----------------------------------------------------------------------------\n"
        main_code += "// user event\n"
        main_code += "// ----------------------------------------------------------------------------\n\n"
        
        event_code = ""
        event_code += "#define TOTAL_USR_EVENT_COUNT {}\n\n".format(len(self.events))
        event_code += "static DataTemplateEvent sg_usr_data_template_event[TOTAL_USR_EVENT_COUNT];\n\n"
        event_code += "static void _init_data_template_event(void)\n"
        event_code += "{\n"

        for event  in self.events:
            main_code += event.create_event_params_global_var()
            static_usr_event_array_name = "sg_usr_data_template_event[{}]".format(event.get_event_enum_index())
            event_code += "    {}.event_id = \"{}\";\n".format(static_usr_event_array_name, event.event_id)
            event_code += "    {}.type = {};\n".format(static_usr_event_array_name, event.event_type_enum)
            event_code += "    {}.params = sg_usr_event_{}_params;\n\n".format(static_usr_event_array_name, event.event_id)
        event_code += "}\n\n"

        main_code += event_code
        # print(event_params)
        return main_code

    def __action_data_initializer(self):
        main_code = ""

        action_code = ""
        main_code += "// ----------------------------------------------------------------------------\n"
        main_code += "// user action\n"
        main_code += "// ----------------------------------------------------------------------------\n\n"

        action_code += "#define TOTAL_USR_ACTION_COUNT {}\n\n".format(len(self.actions))
        action_code += "static DataTemplateAction sg_usr_data_template_action[TOTAL_USR_ACTION_COUNT];\n\n"
        action_code += "static void _init_data_template_action(void)\n"
        action_code += "{\n"

        for action in self.actions:
            main_code += action.create_action_input_init()
            main_code += action.create_action_reply()

            static_usr_action_input_array_name = "sg_usr_data_template_action[{}]".format(action.get_action_enum_index())
            action_code += "    _init_data_template_action_{}_input();\n".format(action.action_id)
            action_code += "    {}.action_id = \"{}\";\n".format(static_usr_action_input_array_name, action.action_id)
            action_code += "    {}.input_struct.value_struct.property = sg_usr_action_{}_input;\n".format(static_usr_action_input_array_name,action.action_id)
            action_code += "    {}.input_struct.value_struct.count = TOTAL_USR_ACTION_{}_INPUT_PARAMS_COUNT;\n".format(static_usr_action_input_array_name,action.action_id.upper())
            action_code += "    {}.reply = sg_usr_action_{}_reply;\n\n".format(static_usr_action_input_array_name, action.action_id)
        action_code += "}\n\n"

        main_code += action_code
        return main_code

    def gen_config_header(self):
        main_code = ""
        main_code += self.__index_enum_create(self.properties, "property")
        main_code += self.__index_enum_create(self.events, "event")
        main_code += self.__index_enum_create(self.actions, "action")
        return main_code

    def gen_config_src_c(self):
        main_code = ""
        main_code += self.__property_data_initializer()
        main_code += self.__event_data_initializer()
        main_code += self.__action_data_initializer()
        return main_code

def main():
    parser = argparse.ArgumentParser(description='Iot hub data_template and events config code generator.',
                                     usage='use "./data_template_codegen.py -c xx/config.json" gen config code')
    parser.add_argument('-c', '--config', dest='config', metavar='xxx.json', required=False, default='xxx.json',
                        help='copy the generated file (data_config.c and events_config.c) to data_template_sample dir '
                             'or your own code dir with data_template. '
                        '\nconfig file can be download from tencent iot-hub platfrom. https://console.cloud.tencent.com/iotcloud')
    parser.add_argument('-d', '--dest', dest='dest', required=False, default='.',
                        help='Dest directory for generated code files, no / at the end.')
    args = parser.parse_args()

    config_path = args.config
    if not os.path.exists(config_path):
        print(u"错误：配置文件不存在，请重新指定数据模板配置文件路径,请参考用法 ./data_template_code_generate.py -c <dir>/data_template.json".format(config_path))
        return 1

    config_dir = os.path.dirname(config_path)
    if config_dir:
        config_dir += "/"

    f = open(config_path, "r", encoding='utf-8')
    try:
        thing_model = json.load(f)
        if 'properties' not in thing_model:
            thing_model.properties = []

        if 'events' not in thing_model:
            thing_model.events = []

        print(u"加载 {} 文件成功".format(config_path))
    except ValueError as e:
        print(u"错误：文件格式非法，请检查 {} 文件是否是 JSON 格式。".format(config_path))
        return 1

    try:
        snippet = iot_struct(thing_model)

        output_config_header_file_name = args.dest + "/data_template_config_header.include"
        with open(output_config_header_file_name, "w") as file:
            file.write("{}".format(snippet.gen_config_header()))
            file.close()
            print(u"文件 {} 生成成功".format(output_config_header_file_name))

        output_config_src_c_file_name = args.dest + "/data_template_config_src_c.include"
        with open(output_config_src_c_file_name, "w") as file:
            file.write("{}".format(snippet.gen_config_src_c()))
            file.close()
            print(u"文件 {} 生成成功".format(output_config_src_c_file_name))
        return 0
    except ValueError as e:
        print(e)
        return 1

if __name__ == '__main__':
    sys.exit(main())
