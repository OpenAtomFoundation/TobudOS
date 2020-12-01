#!/usr/bin/python
# -*- coding: UTF-8 -*-
import sys
import json
import os
import configparser

script_path = os.path.split(os.path.realpath(__file__))[0]
dt_config = configparser.ConfigParser()


def _dt_get_config_from_file():
    config_file = open(os.path.join(script_path, '..', 'config', 'dt.conf'), 'r', encoding='UTF-8')
    config_content = config_file.read()
    config_file.close()
    dt_config.read_string(config_content)
    pass


def _dt_get_json_from_file(filenmae):
    _dt_get_config_from_file()
    json_file = open(filenmae, 'r', encoding='UTF-8')
    file_content = json_file.read()
    json_file.close()
    return json.loads(file_content)


def _dt_write_newline_to_file(write_fd, write_buf):
    write_fd.writelines(write_buf + '\n')
    pass


def _dt_write_macro_to_file(write_fd, macro, macro_val):
    write_fd.writelines('#define\t%-40s\t%-32s\n' % (dt_config['FILE']['MACRO_PREFIX'] +
                                                     macro.upper(), macro_val.upper()))
    pass


def _dt_write_enum_to_file(write_fd, enum_comments, enum_prefix, enum_suffix_list, enum_value_list=None):
    init_flag = True
    write_fd.writelines('\n' + enum_comments + '\n')
    write_fd.writelines('enum {\n')

    if enum_value_list == None:
        for enum_suffix in enum_suffix_list:
            if init_flag:
                write_fd.writelines('\t' + dt_config['FILE']['ENUM_PREFIX'] +
                                    (enum_prefix + '_' + enum_suffix).upper() + ' = 0,\n')
                init_flag = False
            else:
                write_fd.writelines('\t' + dt_config['FILE']['ENUM_PREFIX'] +
                                    (enum_prefix + '_' + enum_suffix).upper() + ',\n')
    else:
        for idx, enum_suffix in enumerate(enum_suffix_list):
            write_fd.writelines('\t' + dt_config['FILE']['ENUM_PREFIX'] +
                                (enum_prefix + '_' + enum_suffix).upper() + ' = %s,\n' % enum_value_list[idx])

    write_fd.writelines('};\n')
    pass


def _dt_get_enum_list_from_conf(conf_data):
    return [v for k, v in conf_data.items()]


def _dt_get_enum_list_from_ids(property_json):
    id_list = [value.get('id') for value in property_json]
    id_list.append('BUTT')
    return id_list


def _dt_get_enum_list_from_mapping(mapping):
    new_list = sorted(mapping.keys())
    enum_prefix = [mapping[key] for key in new_list]
    enum_prefix.append('BUTT')
    enum_val = [key for key in new_list]
    enum_val.append(str(int(new_list[-1]) + 1))

    return [enum_prefix, enum_val]


def _dt_not_exist(dt_data):
    if not dt_data:
        return True
    else:
        return False


def _dt_write_public_header(write_fd):
    _dt_write_enum_to_file(write_fd, '// data type in template, corresponding to type in json file',
                           dt_config['DATA_TYPE'].name,
                           _dt_get_enum_list_from_conf(dt_config['DATA_TYPE']))
    _dt_write_enum_to_file(write_fd, '// message type, reference data template ', dt_config['MSG_TYPE'].name,
                           _dt_get_enum_list_from_conf(dt_config['MSG_TYPE']))
    _dt_write_enum_to_file(write_fd, '// define property authority, not used', dt_config['PROPERTY_AUTH'].name,
                           _dt_get_enum_list_from_conf(dt_config['PROPERTY_AUTH']))
    _dt_write_enum_to_file(write_fd, '// define reply result', dt_config['REPLY'].name,
                           _dt_get_enum_list_from_conf(dt_config['REPLY']))
    _dt_write_enum_to_file(write_fd, '// define message flow direction', dt_config['EFFECT'].name,
                           _dt_get_enum_list_from_conf(dt_config['EFFECT']))
    _dt_write_enum_to_file(write_fd, '// define message type that from server to device', dt_config['DATA_DOWN'].name,
                           _dt_get_enum_list_from_conf(dt_config['DATA_DOWN']))
    _dt_write_enum_to_file(write_fd, '// define message type that from device to server', dt_config['EVENT_UP'].name,
                           _dt_get_enum_list_from_conf(dt_config['EVENT_UP']))

    _dt_write_newline_to_file(write_fd,
                              '\n// msg header define, bit 7-6 is msg type, bit 5 means request or reply, bit 4 - 0 is id')
    _dt_write_macro_to_file(write_fd, 'PARSE_MSG_HEAD_TYPE(_c)', '(((_c) & 0xFF) >> 6)')
    _dt_write_macro_to_file(write_fd, 'PARSE_MSG_HEAD_EFFECT(_c)',
                            '((((_c) & 0xFF) & 0x20) ? BLE_QIOT_EFFECT_REPLY : BLE_QIOT_EFFECT_REQUEST)')
    _dt_write_macro_to_file(write_fd, 'PARSE_MSG_HEAD_ID(_c)', '((_c) & 0x1F)')
    _dt_write_macro_to_file(write_fd, 'PACKAGE_MSG_HEAD(_type, _reply, _id)',
                            '(((_type) << 6) | (((_reply) == BLE_QIOT_EFFECT_REPLY) << 5) | ((_id) & 0x1F))')

    _dt_write_newline_to_file(write_fd,
                              '\n// tlv header define, bit 7 - 5 is type, bit 4 - 0 depends on type of data template')
    _dt_write_macro_to_file(write_fd, 'PARSE_TLV_HEAD_TYPE(_c)', '(((_c) & 0xFF) >> 5)')
    _dt_write_macro_to_file(write_fd, 'PARSE_TLV_HEAD_ID(_c)', '((_c) & 0x1F)')
    _dt_write_macro_to_file(write_fd, 'PACKAGE_TLV_HEAD(_type, _id)', '(((_type) << 5) | ((_id) & 0x1F))\n')

    _dt_write_newline_to_file(write_fd, '\n// define tlv struct')
    _dt_write_newline_to_file(write_fd, 'typedef struct{'
                                        '\n\tuint8_t type;'
                                        '\n\tuint8_t id;'
                                        '\n\tuint16_t len;'
                                        '\n\tchar *val;'
                                        '\n}e_ble_tlv;')
    pass


def _dt_trans_property_json_to_h_file(write_fd, _proterty_data):
    if _dt_not_exist(_proterty_data):
        return

    _dt_write_macro_to_file(write_fd, 'INCLUDE_PROPERTY', '')
    # all property id define, the tail of macro name corresponding to property id in json file
    _dt_write_enum_to_file(write_fd, '// define property id', 'PROPERTY_ID', _dt_get_enum_list_from_ids(_proterty_data))

    # define property id values, including size, length, limit ...
    for property_id in _proterty_data:
        if property_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['ENUM']:
            enum_prefix, enum_val = _dt_get_enum_list_from_mapping(
                property_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['MAPPING']])
            _dt_write_enum_to_file(write_fd, '// define property %s enum' % property_id[dt_config['JSON']['ID']],
                                   'PROPERTY_' + property_id[dt_config['JSON']['ID']], enum_prefix, enum_val)
        elif property_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['INT'] or \
            property_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['FLOAT']:
            _dt_write_newline_to_file(write_fd, '\n// define %s attributes' % property_id[dt_config['JSON']['ID']])
            for k, v in property_id[dt_config['JSON']['DEFINE']].items():
                if v != '' and k != dt_config['JSON']['TYPE'] and k != dt_config['JSON']['UNIT']:
                    _dt_write_macro_to_file(write_fd, 'PROPERTY_' + property_id[dt_config['JSON']['ID']] + '_' + k,
                                            '(' + v + ')')
        elif property_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['STRING']:
            _dt_write_newline_to_file(write_fd, '\n// define %s length limit' % property_id[dt_config['JSON']['ID']])
            for k, v in property_id[dt_config['JSON']['DEFINE']].items():
                if v != '' and k != dt_config['JSON']['TYPE']:
                    _dt_write_macro_to_file(write_fd,
                                            'PROPERTY_' + property_id[dt_config['JSON']['ID']] + '_LEN_' + k,
                                            '(' + v + ')')
        else:
            pass
    # define prototype of callback function
    _dt_write_newline_to_file(write_fd, '\n// define property set handle return 0 if success, other is error\n'
                                        '// sdk call the function that inform the server data to the device')
    _dt_write_newline_to_file(write_fd, 'typedef int (*property_set_cb)(const char *data, uint16_t len);')
    _dt_write_newline_to_file(write_fd,
                              '\n// define property get handle. return the data length obtained, -1 is error, 0 is no data\n'
                              '// sdk call the function fetch user data and send to the server, the data should wrapped by user '
                              'adn skd just transmit')
    _dt_write_newline_to_file(write_fd, 'typedef int (*property_get_cb)(char *buf, uint16_t buf_len);')
    _dt_write_newline_to_file(write_fd,
                              '\n// each property have a struct ble_property_t, make up a array named sg_ble_property_array')
    _dt_write_newline_to_file(write_fd, 'typedef struct{'
                                        '\n\tproperty_set_cb set_cb;\t//set callback'
                                        '\n\tproperty_get_cb get_cb;\t//get callback'
                                        '\n\tuint8_t authority;\t//property authority'
                                        '\n\tuint8_t type;\t//data type'
                                        '\n}ble_property_t;')
    pass


def _dt_trans_event_json_to_h_file(write_fd, _event_data):
    if _dt_not_exist(_event_data):
        return

    _dt_write_macro_to_file(write_fd, 'INCLUDE_EVENT', '')
    # define event id
    _dt_write_enum_to_file(write_fd, '// define event id', 'EVENT_ID', _dt_get_enum_list_from_ids(_event_data))

    for event in _event_data:
        # define param id of event
        _dt_write_enum_to_file(write_fd, '// define param id for event %s' % event.get(dt_config['JSON']['ID']),
                               'EVENT_' + event.get(dt_config['JSON']['ID']) + '_PARAM_ID',
                               _dt_get_enum_list_from_ids(event.get(dt_config['JSON']['PARAMS'])))
        # define param value of event
        for param in event.get('params'):
            if param[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['ENUM']:
                enum_prefix, enum_val = _dt_get_enum_list_from_mapping(param[dt_config['JSON']['DEFINE']]['mapping'])
                _dt_write_enum_to_file(write_fd, '// define enum for param %s' % param[dt_config['JSON']['ID']],
                                       'EVEMT_' + event.get(dt_config['JSON']['ID']) + '_' + param[
                                           dt_config['JSON']['ID']],
                                       enum_prefix, enum_val)
            elif param[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['INT'] or \
                param[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['FLOAT']:
                _dt_write_newline_to_file(write_fd, '\n// define param %s attributes' % param[dt_config['JSON']['ID']])
                for k, v in param[dt_config['JSON']['DEFINE']].items():
                    if v != '' and k != dt_config['JSON']['TYPE'] and k != dt_config['JSON']['UNIT']:
                        _dt_write_macro_to_file(write_fd,
                                                'EVENT_' + event.get(dt_config['JSON']['ID']) + '_' + param[
                                                    dt_config['JSON']['ID']] + '_' + k,
                                                '(' + v + ')')
            elif param[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['STRING']:
                _dt_write_newline_to_file(write_fd, '\n// define range for param %s' % param[dt_config['JSON']['ID']])
                for k, v in param[dt_config['JSON']['DEFINE']].items():
                    if v != '' and k != dt_config['JSON']['TYPE']:
                        _dt_write_macro_to_file(write_fd,
                                                'EVENT_' + event.get(dt_config['JSON']['ID']) + '_' + param[
                                                    dt_config['JSON']['ID']] + '_LEN_' + k,
                                                '(' + v + ')')

    _dt_write_newline_to_file(write_fd,
                              '\n// define event get handle. return the data length obtained, -1 is error, 0 is no data\n'
                              '// sdk call the function fetch user data and send to the server, the data should wrapped by user '
                              'adn skd just transmit')
    _dt_write_newline_to_file(write_fd, 'typedef int (*event_get_cb)(char *buf, uint16_t buf_len);')
    _dt_write_newline_to_file(write_fd, '\n// each param have a struct ble_event_param, make up a array for the event')
    _dt_write_newline_to_file(write_fd, 'typedef struct{'
                                        '\n\tevent_get_cb get_cb;\t//get param data callback'
                                        '\n\tuint8_t type;\t//param type'
                                        '\n}ble_event_param;')
    _dt_write_newline_to_file(write_fd, '\n// a array named sg_ble_event_array is composed by all the event array')
    _dt_write_newline_to_file(write_fd, 'typedef struct{'
                                        '\n\tble_event_param *event_array;\t//array of params data'
                                        '\n\tuint8_t array_size;\t//array size'
                                        '\n}ble_event_t;')
    pass


def _dt_trans_action_json_to_h_file(write_fd, _action_data):
    if _dt_not_exist(_action_data):
        return

    _dt_write_macro_to_file(write_fd, 'INCLUDE_ACTION', '')
    # define action id
    _dt_write_enum_to_file(write_fd, '// define action id', 'ACTION_ID', _dt_get_enum_list_from_ids(_action_data))
    max_input_id, max_output_id = 0, 0
    for action in _action_data:
        # define action input id
        _dt_write_enum_to_file(write_fd, '// define input id for action %s' % action.get(dt_config['JSON']['ID']),
                               'ACTION_' + action.get(dt_config['JSON']['ID']) + '_INPUT_ID',
                               _dt_get_enum_list_from_ids(action.get(dt_config['JSON']['INPUT'])))
        _dt_write_enum_to_file(write_fd, '// define output id for action %s' % action.get(dt_config['JSON']['ID']),
                               'ACTION_' + action.get(dt_config['JSON']['ID']) + '_OUTPUT_ID',
                               _dt_get_enum_list_from_ids(action.get(dt_config['JSON']['OUTPUT'])))
        max_input_id = max(len(_dt_get_enum_list_from_ids(action.get(dt_config['JSON']['INPUT']))), max_input_id)
        max_output_id = max(len(_dt_get_enum_list_from_ids(action.get(dt_config['JSON']['OUTPUT']))), max_output_id)
        # define input id values
        for input_id in action.get('input'):
            if input_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['ENUM']:
                enum_prefix, enum_val = _dt_get_enum_list_from_mapping(
                    input_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['MAPPING']])
                _dt_write_enum_to_file(write_fd, '// define enum for input id %s' % input_id[dt_config['JSON']['ID']],
                                       'ACTION_INPUT_' + action.get(dt_config['JSON']['ID']) + '_' + input_id[
                                           dt_config['JSON']['ID']],
                                       enum_prefix, enum_val)
            elif input_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['INT'] or \
                input_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['FLOAT']:
                for k, v in input_id[dt_config['JSON']['DEFINE']].items():
                    if v != '' and k != dt_config['JSON']['TYPE'] and k != dt_config['JSON']['UNIT']:
                        _dt_write_macro_to_file(write_fd,
                                                'ACTION_INPUT_' + action.get(dt_config['JSON']['ID']) + '_' + input_id[
                                                    dt_config['JSON']['ID']] + '_' + k,
                                                '(' + v + ')')
            elif input_id[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['STRING']:
                _dt_write_newline_to_file(write_fd,
                                          '\n// define input id %s attributes' % input_id[dt_config['JSON']['ID']])
                for k, v in input_id[dt_config['JSON']['DEFINE']].items():
                    if v != '' and k != dt_config['JSON']['TYPE']:
                        _dt_write_macro_to_file(write_fd,
                                                'ACTION_OUTPUT_' + action.get(dt_config['JSON']['ID']) + '_' + input_id[
                                                    dt_config['JSON']['ID']] + '_LEN_' + k,
                                                '(' + v + ')')
            else:
                pass
        for output in action.get('output'):
            if output[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['ENUM']:
                enum_prefix, enum_val = _dt_get_enum_list_from_mapping(
                    output[dt_config['JSON']['DEFINE']][dt_config['JSON']['MAPPING']])
                _dt_write_enum_to_file(write_fd, '// define enum for output id %s' % output[dt_config['JSON']['ID']],
                                       'ACTION_OUTPUT_' + action.get(dt_config['JSON']['ID']) + '_' + output[
                                           dt_config['JSON']['ID']],
                                       enum_prefix, enum_val)
            elif output[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['INT'] or \
                output[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['FLOAT']:
                _dt_write_newline_to_file(write_fd,
                                          '\n// define output id %s attributes' % output[dt_config['JSON']['ID']])
                for k, v in output[dt_config['JSON']['DEFINE']].items():
                    if v != '' and k != dt_config['JSON']['TYPE'] and k != dt_config['JSON']['UNIT']:
                        _dt_write_macro_to_file(write_fd,
                                                'ACTION_OUTPUT_' + action.get(dt_config['JSON']['ID']) + '_' + output[
                                                    dt_config['JSON']['ID']] + '_' + k,
                                                '(' + v + ')')
            elif output[dt_config['JSON']['DEFINE']][dt_config['JSON']['TYPE']] == dt_config['JSON']['STRING']:
                _dt_write_newline_to_file(write_fd,
                                          '\n// define output id %s attributes' % output[dt_config['JSON']['ID']])
                for k, v in output[dt_config['JSON']['DEFINE']].items():
                    if v != '' and k != dt_config['JSON']['TYPE']:
                        _dt_write_macro_to_file(write_fd,
                                                'ACTION_OUTPUT_' + action.get(dt_config['JSON']['ID']) + '_' + output[
                                                    dt_config['JSON']['ID']] + '_LEN_' + k,
                                                '(' + v + ')')
    _dt_write_newline_to_file(write_fd, '\n// define max input id and output id in all of input id and output id above')
    _dt_write_macro_to_file(write_fd, 'ACTION_INPUT_ID_BUTT', str(max_input_id - 1))
    _dt_write_macro_to_file(write_fd, 'ACTION_OUTPUT_ID_BUTT', str(max_output_id - 1))
    _dt_write_newline_to_file(write_fd, '\n// define action input handle, return 0 is success, other is error.\n'
                                        '// input_param_array carry the data from server, include input id, data length ,data val\n'
                                        '// input_array_size means how many input id\n'
                                        '// output_id_array filling with output id numbers that need obtained, sdk will traverse it and call the action_output_handle to obtained data')
    _dt_write_newline_to_file(write_fd,
                              'typedef int (*action_input_handle)(e_ble_tlv *input_param_array, uint8_t input_array_size, uint8_t *output_id_array);')
    _dt_write_newline_to_file(write_fd,
                              '\n// define action output handle, return length of the data, 0 is no data, -1 is error\n'
                              '// output_id means which id data should be obtained')
    _dt_write_newline_to_file(write_fd,
                              'typedef int (*action_output_handle)(uint8_t output_id, char *buf, uint16_t buf_len);')
    _dt_write_newline_to_file(write_fd,
                              '\n// each action have a struct ble_action_t, make up a array named sg_ble_action_array')
    _dt_write_newline_to_file(write_fd, 'typedef struct{'
                                        '\n\taction_input_handle input_cb;\t//handle input data'
                                        '\n\taction_output_handle output_cb;\t// get output data in the callback'
                                        '\n\tuint8_t *input_type_array;\t//type array for input id'
                                        '\n\tuint8_t *output_type_array;\t//type array for output id'
                                        '\n\tuint8_t input_id_size;\t//numbers of input id'
                                        '\n\tuint8_t output_id_size;\t//numbers of output id'
                                        '\n}ble_action_t;')
    pass


def _dt_write_function_prototype(write_fd):
    file_fd = open(os.path.join(script_path, 'dt_fixed_content', 'dt_ble_prototype'), 'r', encoding='UTF-8')
    file_content = file_fd.read()
    file_fd.close()
    _dt_write_newline_to_file(write_fd, file_content)
    pass


def _dt_generate_header_file(_json_data):
    h_file = open(dt_config['FILE']['NAME_PREFIX'] + '.h', 'w', encoding='UTF-8')

    _dt_write_newline_to_file(h_file, dt_config['FILE']['COPYRIGHT'])
    _dt_write_newline_to_file(h_file, '#ifndef ' + dt_config['FILE']['NAME_PREFIX'].upper() + '_H_')
    _dt_write_newline_to_file(h_file, '#define ' + dt_config['FILE']['NAME_PREFIX'].upper() + '_H_')
    _dt_write_newline_to_file(h_file, '#ifdef __cplusplus\n' + 'extern "C"{\n#endif\n')

    _dt_write_newline_to_file(h_file, '#include <stdint.h>\n')

    _dt_write_public_header(h_file)
    _dt_trans_property_json_to_h_file(h_file, _json_data.get(dt_config['JSON']['PROPERTY']))
    _dt_trans_event_json_to_h_file(h_file, _json_data.get(dt_config['JSON']['EVENT']))
    _dt_trans_action_json_to_h_file(h_file, _json_data.get(dt_config['JSON']['ACTION']))
    _dt_write_function_prototype(h_file)

    _dt_write_newline_to_file(h_file, '\n' + '#ifdef __cplusplus' + '\n' + '}' + '\n' + '#endif')
    _dt_write_newline_to_file(h_file, '#endif ' + '//' + (dt_config['FILE']['NAME_PREFIX']).upper() + '_H_')

    h_file.close()
    pass


def _dt_get_property_function_name(property_id):
    return 'ble_property_' + property_id.lower() + '_get'


def _dt_set_property_function_name(property_id):
    return 'ble_property_' + property_id.lower() + '_set'


def _dt_get_property_mode(mode):
    if mode == 'rw':
        return 'BLE_QIOT_PROPERTY_AUTH_RW'
    elif mode == 'r':
        return 'BLE_QIOT_PROPERTY_AUTH_READ'
    else:
        print("invalid property mode")
        pass


def _dt_get_type_by_str(type_str):
    if type_str == dt_config['JSON']['BOOL']:
        return 'BLE_QIOT_DATA_TYPE_BOOL'
    elif type_str == dt_config['JSON']['INT']:
        return 'BLE_QIOT_DATA_TYPE_INT'
    elif type_str == dt_config['JSON']['STRING']:
        return 'BLE_QIOT_DATA_TYPE_STRING'
    elif type_str == dt_config['JSON']['FLOAT']:
        return 'BLE_QIOT_DATA_TYPE_FLOAT'
    elif type_str == dt_config['JSON']['ENUM']:
        return 'BLE_QIOT_DATA_TYPE_ENUM'
    elif type_str == dt_config['JSON']['TIME']:
        return 'BLE_QIOT_DATA_TYPE_TIME'
    else:
        print('invalid type string')
    pass


def _dt_get_ret_val_by_type(data_type):
    if data_type == dt_config['JSON']['STRING']:
        return 'buf_len'
    elif data_type == dt_config['JSON']['INT']:
        return 'sizeof(uint32_t)'
    elif data_type == dt_config['JSON']['ENUM']:
        return 'sizeof(uint16_t)'
    elif data_type == dt_config['JSON']['BOOL']:
        return 'sizeof(uint8_t)'
    elif data_type == dt_config['JSON']['FLOAT']:
        return 'sizeof(float)'
    elif data_type == dt_config['JSON']['TIME']:
        return 'sizeof(uint32_t)'
    else:
        print('invalid data type')


def _dt_get_function_param_by_type(data_type):
    return '(char *data, uint16_t buf_len)'


def _dt_trans_property_json_to_c_file(write_fd, _proterty_data):
    if _dt_not_exist(_proterty_data):
        return

    # define property set/get function
    for property_id in _proterty_data:
        _dt_write_newline_to_file(write_fd,
                                  'static int ' + _dt_set_property_function_name(
                                      property_id.get(dt_config['JSON']['ID'])) +
                                  '(const char *data, uint16_t len)\n{\n\treturn 0;\n}\n')
        _dt_write_newline_to_file(write_fd,
                                  'static int ' + _dt_get_property_function_name(
                                      property_id.get(dt_config['JSON']['ID'])) +
                                  '%s\n{\n\treturn %s;\n}\n' %
                                  (_dt_get_function_param_by_type(
                                      property_id.get(dt_config['JSON']['DEFINE']).get(dt_config['JSON']['TYPE'])),
                                   _dt_get_ret_val_by_type(
                                       property_id.get(dt_config['JSON']['DEFINE']).get(dt_config['JSON']['TYPE']))))

    # define property array
    _dt_write_newline_to_file(write_fd, 'static ble_property_t sg_ble_property_array[BLE_QIOT_PROPERTY_ID_BUTT] = {')
    for property_id in _proterty_data:
        _dt_write_newline_to_file(write_fd, '\t{%-30s %-30s %-20s %-20s},' %
                                  (_dt_set_property_function_name(property_id.get(dt_config['JSON']['ID'])) + ',',
                                   _dt_get_property_function_name(property_id.get(dt_config['JSON']['ID'])) + ',',
                                   _dt_get_property_mode(property_id.get(dt_config['JSON']['MODE'])) + ',',
                                   _dt_get_type_by_str(
                                       property_id.get(dt_config['JSON']['DEFINE']).get(dt_config['JSON']['TYPE']))
                                   ))
    _dt_write_newline_to_file(write_fd, '};\n')

    dt_property_file = open(os.path.join(script_path, 'dt_fixed_content', 'dt_ble_property'), 'r', encoding='UTF-8')
    dt_property_content = dt_property_file.read()
    dt_property_file.close()
    # write operation to file
    _dt_write_newline_to_file(write_fd, dt_property_content)
    pass


def _dt_trans_event_json_to_c_file(write_fd, _evnet_data):
    if _dt_not_exist(_evnet_data):
        return

    for event in _evnet_data:
        # define event get function
        for param in event.get('params'):
            _dt_write_newline_to_file(write_fd,
                                      'static int ' + 'ble_event_get_' + event.get(dt_config['JSON']['ID']).lower() +
                                      '_' + param.get(dt_config['JSON']['ID']).lower() + '%s\n{\n\treturn %s;\n}\n' %
                                      (_dt_get_function_param_by_type(
                                          param.get(dt_config['JSON']['DEFINE']).get(dt_config['JSON']['TYPE'])),
                                       _dt_get_ret_val_by_type(
                                           param.get(dt_config['JSON']['DEFINE']).get(dt_config['JSON']['TYPE']))))
        # define param array
        _dt_write_newline_to_file(write_fd, 'static ble_event_param sg_ble_event_%s_array[%s] = {' %
                                  (event.get(dt_config['JSON']['ID']).lower(),
                                   dt_config['FILE']['ENUM_PREFIX'] + 'EVENT_' +
                                   event.get(dt_config['JSON']['ID']).upper() + '_PARAM_ID_BUTT'))
        for param in event.get('params'):
            _dt_write_newline_to_file(write_fd, '\t{%-32s %-20s},' % (
                'ble_event_get_' + event.get(dt_config['JSON']['ID']).lower() + '_' + param.get(
                    dt_config['JSON']['ID']).lower() + ', ',
                _dt_get_type_by_str(param.get(dt_config['JSON']['DEFINE']).get(dt_config['JSON']['TYPE']))))
        _dt_write_newline_to_file(write_fd, '};\n')

    # define event array
    _dt_write_newline_to_file(write_fd, 'static ble_event_t sg_ble_event_array[BLE_QIOT_EVENT_ID_BUTT] = {')
    for event in _evnet_data:
        _dt_write_newline_to_file(write_fd, '\t{%-32s %-32s},' % (
            'sg_ble_event_' + event.get(dt_config['JSON']['ID']).lower() + '_array,',
            'sizeof(sg_ble_event_' + event.get(dt_config['JSON']['ID']).lower() + '_array) / sizeof(ble_event_param)'))
    _dt_write_newline_to_file(write_fd, '};\n')

    dt_event_file = open(os.path.join(script_path, 'dt_fixed_content', 'dt_ble_event'), 'r', encoding='UTF-8')
    dt_event_content = dt_event_file.read()
    dt_event_file.close()
    # write operation to file
    _dt_write_newline_to_file(write_fd, dt_event_content)
    pass


def _dt_trans_action_json_to_c_file(write_fd, _action_data):
    if _dt_not_exist(_action_data):
        return

    for idx, action in enumerate(_action_data):
        # define action input and output callback
        _dt_write_newline_to_file(write_fd,
                                  'static int ' + 'ble_action_handle_' + action.get(
                                      dt_config['JSON']['ID']).lower() + '_input_cb' +
                                  '(e_ble_tlv *input_param_array, uint8_t input_array_size, uint8_t *output_id_array)\n'
                                  '{\n\treturn 0;\n}\n')
        _dt_write_newline_to_file(write_fd,
                                  'static int ' + 'ble_action_handle_' + action.get(
                                      dt_config['JSON']['ID']).lower() + '_output_cb' +
                                  '(uint8_t output_id, char *buf, uint16_t buf_len)\n'
                                  '{\n\treturn buf_len;\n}\n')
        # define type array of input id
        _dt_write_newline_to_file(write_fd,
                                  'static uint8_t ' + 'sg_ble_action_%s_input_type_array[%s] = {' %
                                  (action.get(dt_config['JSON']['ID']).lower(),
                                   dt_config['FILE']['ENUM_PREFIX'] + 'ACTION_' +
                                   action.get(dt_config['JSON']['ID']).upper() + '_INPUT_ID_BUTT'))
        for input_id in action.get('input'):
            _dt_write_newline_to_file(write_fd,
                                      '\t' + _dt_get_type_by_str(input_id.get(dt_config['JSON']['DEFINE']).get(
                                          dt_config['JSON']['TYPE'])) + ',')
        _dt_write_newline_to_file(write_fd, '};\n')

        # define type array of output id
        _dt_write_newline_to_file(write_fd,
                                  'static uint8_t ' + 'sg_ble_action_%s_output_type_array[%s] = {' % (
                                      action.get(dt_config['JSON']['ID']).lower(),
                                      dt_config['FILE']['ENUM_PREFIX'] + 'ACTION_' + action.get(
                                          dt_config['JSON']['ID']).upper() + '_OUTPUT_ID_BUTT'))
        for input_id in action.get('output'):
            _dt_write_newline_to_file(write_fd,
                                      '\t' + _dt_get_type_by_str(input_id.get(dt_config['JSON']['DEFINE']).get(
                                          dt_config['JSON']['TYPE'])) + ',')
        _dt_write_newline_to_file(write_fd, '};\n')

    # define action array
    _dt_write_newline_to_file(write_fd, 'static ble_action_t ' + 'sg_ble_action_array[BLE_QIOT_ACTION_ID_BUTT] = {')
    for action in _action_data:
        _dt_write_newline_to_file(write_fd,
                                  '\t{ble_action_handle_%s_input_cb, ble_action_handle_%s_output_cb, \n'
                                  '\t\tsg_ble_action_%s_input_type_array, sg_ble_action_%s_output_type_array, \n'
                                  '\t\tsizeof(sg_ble_action_%s_input_type_array) / sizeof(uint8_t), \n'
                                  '\t\tsizeof(sg_ble_action_%s_output_type_array) / sizeof(uint8_t)},' %
                                  (action.get(dt_config['JSON']['ID']).lower(),
                                   action.get(dt_config['JSON']['ID']).lower(),
                                   action.get(dt_config['JSON']['ID']).lower(),
                                   action.get(dt_config['JSON']['ID']).lower(),
                                   action.get(dt_config['JSON']['ID']).lower(),
                                   action.get(dt_config['JSON']['ID']).lower()))
    _dt_write_newline_to_file(write_fd, '};\n')

    dt_action_file = open(os.path.join(script_path, 'dt_fixed_content', 'dt_ble_action'), 'r', encoding='UTF-8')
    dt_action_content = dt_action_file.read()
    dt_action_file.close()
    # write operation to file
    _dt_write_newline_to_file(write_fd, dt_action_content)
    pass


def _dt_generate_c_file(_json_data):
    c_file = open(dt_config['FILE']['NAME_PREFIX'] + '.c', 'w', encoding='UTF-8')

    _dt_write_newline_to_file(c_file, dt_config['FILE']['COPYRIGHT'])
    _dt_write_newline_to_file(c_file, '#ifdef __cplusplus\nextern "C" {\n#endif\n')

    _dt_write_newline_to_file(c_file, '#include "ble_qiot_template.h"\n')
    _dt_write_newline_to_file(c_file, '#include <stdio.h>')
    _dt_write_newline_to_file(c_file, '#include <stdbool.h>')
    _dt_write_newline_to_file(c_file, '#include <string.h>\n')
    _dt_write_newline_to_file(c_file, '#include "ble_qiot_export.h"')
    _dt_write_newline_to_file(c_file, '#include "ble_qiot_common.h"')
    _dt_write_newline_to_file(c_file, '#include "ble_qiot_param_check.h"\n')

    _dt_trans_property_json_to_c_file(c_file, _json_data.get(dt_config['JSON']['PROPERTY']))
    _dt_trans_event_json_to_c_file(c_file, _json_data.get(dt_config['JSON']['EVENT']))
    _dt_trans_action_json_to_c_file(c_file, _json_data.get(dt_config['JSON']['ACTION']))
    _dt_write_newline_to_file(c_file, '\n#ifdef __cplusplus\n}\n#endif')
    pass


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print('Usage: python3 %s <json_file>' % sys.argv[0])
    else:
        print("reading json file start")
        json_data = _dt_get_json_from_file(sys.argv[1])
        print("reading json file end")

        print("generate header file start")
        _dt_generate_header_file(json_data)
        print("generate header file end")
        print("generate source file start")
        _dt_generate_c_file(json_data)
        print("generate source file end")
        pass
