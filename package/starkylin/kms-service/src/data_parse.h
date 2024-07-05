#ifndef _PARSE_DATA_H_
#define _PARSE_DATA_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * 解析序列化数据 MessageInfo
 * @param data (const char*) 数据体
 * @param len  数据长度
 */
int decode_msg(const char* data, const size_t len);


#ifdef __cplusplus
}
#endif

#endif /*_PARSE_DATA_H_*/
