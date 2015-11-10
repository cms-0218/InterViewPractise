/* 
 * File:   Tools.cpp
 * Author: caojun
 * 
 * Created on 2015年9月19日, 下午4:14
 */


#include "kpsdefs.h"
#include <iconv.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include<ctype.h> 
#include <iostream>  
#include <sstream> 
#include <unistd.h>
#include "Tools.h"
#include "include/Base64Utils.h"

//字符编码转换（UTF-8/GBK/ISO8859-1）


string Tools::PREFIX_TZDH_BZ = "开具红字增值税专用发票信息表编号";
string Tools::PREFIX_HYTZDH_BZ = "开具红字货物运输业增值税专用发票信息表编号";
string Tools::PREFIX_FPDM_BZ = "对应正数发票代码:";
string Tools::PREFIX_FPHM_BZ = "号码:";
string Tools::PREFIX_DKQYSH = "代开企业税号:";
string Tools::PREFIX_DKQYMC = "代开企业名称:";

int Tools::MAXPERIOD = 1000;

int Tools::ConvertCharSet(const char *dest,
                          const char *src,
                          char *input,
                          size_t inputlen,
                          char *output,
                          size_t outputlen)
{
    iconv_t conv = iconv_open(dest, src);
    if (conv == (iconv_t) - 1)
        return -1;
    memset(output, 0, outputlen);
    if (iconv(conv, &input, &inputlen, &output, &outputlen))
        return -1;
    iconv_close(conv);
    return 0;
}
//int to string

string Tools::Int2Str(int value)
{
    char str[30];
    memset(str, 0, 30);
    sprintf(str, "%d", value);
    string data(str);
    return data;
}

//long to string

string Tools::Long2Str(long value)
{
    char str[30];
    memset(str, 0, 30);
    sprintf(str, "%ld", value);
    string data(str);
    return data;
}
//bool to Str

string Tools::Bool2Int(bool value)
{
    if (value == true)
    {
        return "1";
    }
    else
    {
        return "0";
    }
}



//float to string

string Tools::Float2Str(double value, int precision)
{
    /*
    int prec = 30;//std::numeric_limits<long double>::digits10;	//18
    std::ostringstream out;
    out.precision(prec);
    out<<value;
    return out.str();
     */

    char str[50];
    memset(str, 0, 50);
    sprintf(str, "%.*f", precision, value);
    string data(str);
    //去除多余的0
    while (data[data.length() - 1] == '0')
    {
        data.erase(data.length() - 1);
        //删除最后的.
        if (data[data.length() - 1] == '.')
        {
            data.erase(data.length() - 1);
            break;
        }
    }
    return data;
}
//string to int

int Tools::Str2Int(string value)
{
    if (value.empty())
    {
        value = "0";
    }
    return atoi(value.c_str());
}

//string to long

long Tools::Str2Long(string value)
{
    if (value.empty())
    {
        value = "0";
    }
    return atol(value.c_str());
}

//string to float

double Tools::Str2Float(string value)
{
    if (value.empty())
    {
        value = "0";
    }
    return atof(value.c_str());
}
//四舍五入

double Tools::GetRound(double value, int precision)
{
    if (precision < 0)
        return value;
    int Sign = 1;
    if (value < 0)
        Sign = -1;
    value = fabs(value);
    value += 0.00000000001;
    //用于补浮点误差。
    long long aDb = 1;
    for (int i = 0; i < precision; i++)
    {
        aDb *= 10;
    }

    value *= aDb;
    //value += 0.5000000001;
    value += 0.5000001;
    //用于补浮点误差。
    value = floor(value);
    value = value / (double(aDb));

    //格式字符串
    char buf[100];
    memset(buf, 0, 100);
    //string format = "%."+Int2Str(precision)+"f";
    sprintf(buf,
            "%.*f",
            precision,
            value);
    value = atof(buf);
    return value * Sign;
}
//四舍五入（将string转double，保留precision）

double Tools::GetRound(string value, int precision)
{
    double data = Str2Float(value);
    return GetRound(data, precision);
}

string Tools::formatDoubleString(double doubleValue, int precision)
{
    string value = Tools::Float2Str(doubleValue, precision);
    return formatDoubleString(value, precision);
}

string Tools::formatDoubleString(string value, int precision)
{
    double value2 = Str2Double(value);
    value = Float2Str(value2, precision);
    if (value.find_first_of(".") == -1)
    {
        value = value + ".";
    }
    int pos = value.find_first_of(".");
    int length = value.length();
    //小數位不夠，先增長
    if (length < pos + precision + 2)
    {
        for (int i = 0; i < (pos + precision + 1) - length; i++)
        {
            value = value + "0";
        }
    }
    return value;

}



//字符校验 校验二维码数据里面有没有 GBK字符集范围内的不可见字符

bool Tools::CheckDataStr_GBK(string dataBuf)
{
    bool res = true;

    if (dataBuf.length() <= 0)
        return true;
    int i = 0;
    //GBK,全称全国信息技术标准化技术委员会 汉字内码扩展规范(GBK)
    //GBK 字符集是向下与 GB 2312 编码兼容。
    //其编码范围是高位0x81－0xFE，低位也是0x40-0xFE；
    //GBK：「高位位元組」使用了0x81-0xFE，「低位位元組」使用了0x40-0x7E; 0x80-0xFE
    int datalen = dataBuf.length();
    while (i < datalen)
    {
        //处理\r\n
        if (dataBuf[i] == '\r' || dataBuf[i] == '\n')
        {
            i++;
            continue;
        }
        //GBK 汉字
        if ((unsigned char) dataBuf[i] > 0x80)
        {
            unsigned char HiB = (unsigned char) dataBuf[i];
            i++;
            if (i > datalen) return false;
            unsigned char LoB = (unsigned char) dataBuf[i];
            char buf[20];
            memset(buf, 0, 20);
            sprintf(buf, "%02x %02x", HiB, LoB);
            string logStr("填写的发票有非法字符： ");
            logStr.append(buf);
            //20111221 A8 BC/A8 BF
            if (HiB == 0xA8 && LoB == 0xBC)
            {
                return false;
            }
            if (HiB == 0xA8 && LoB == 0xBF)
            {
                return false;
            }
            //20131108 重庆试运行发现的问题
            if (HiB == 0xA4 && LoB == 0x4B)
            {
                return false;
            }
            //20120104 ＇ 20130530 kps3.0中可以使用
            //else if (HiB == 0xA3 && LoB == 0xA7)
            //{
            //  return false ;
            //}

            if (HiB >= 0x81 && HiB < 0xB0)
            {
                //汉字从0x8140--0xAFFE
                if (LoB > 0xFE || LoB == 0x7F || LoB < 0x40)
                {
                    //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                    return false;
                }
                else
                {
                    if (HiB == 0xA1)
                    {
                        if (LoB <= 0xA1)
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }
                    }
                    else if (HiB == 0xA2)
                    {
                        if ((LoB < 0x40) ||
                                (LoB >= 0xAB && LoB <= 0xB0) ||
                                (LoB >= 0xE3 && LoB <= 0xE4) ||
                                (LoB >= 0xEF && LoB <= 0xF0) ||
                                (LoB >= 0xFD && LoB <= 0xFE))
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }

                    }
                    else if (HiB == 0xA3)
                    {
                        if (LoB > 0xFE || LoB < 0x40)
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }

                    }
                    else if (HiB == 0xA4)
                    {
                        if (LoB >= 0xF4 || LoB < 0x40)
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }
                    }
                    else if (HiB == 0xA5)
                    {
                        if (LoB >= 0xF7 || LoB < 0x40)
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }

                    }
                    else if (HiB == 0xA6)
                    {
                        if ((LoB < 0x40) ||
                                (LoB >= 0xB9 && LoB <= 0xC0) ||
                                (LoB >= 0xD9 && LoB <= 0xDF) ||
                                (LoB == 0xF3) ||
                                (LoB >= 0xF6 && LoB <= 0xFE))
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }
                    }
                    else if (HiB == 0xA7)
                    {
                        if ((LoB < 0x40) ||
                                (LoB >= 0xC2 && LoB <= 0xD0) ||
                                (LoB >= 0xF2 && LoB <= 0xFE))
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }
                    }
                    else if (HiB == 0xA8)
                    {
                        if ((LoB < 0x40) ||
                                (LoB >= 0x96 && LoB <= 0xA0) ||
                                (LoB >= 0xC1 && LoB <= 0xC4) ||
                                (LoB >= 0xEA && LoB <= 0xFE))
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }
                    }
                    else if (HiB == 0xA9)
                    {
                        if ((LoB < 0x40) ||
                                (LoB == 0x58) ||
                                (LoB >= 0x5D && LoB <= 0x5F) ||
                                (LoB >= 0x97 && LoB <= 0xA3) ||
                                (LoB >= 0xF5 && LoB <= 0xFE))
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }
                    }
                    else if (HiB >= 0xAA && HiB <= 0xAF)
                    {
                        if ((LoB < 0x40) ||
                                (LoB > 0xA0))
                        {
                            //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                            return false;
                        }
                    }
                }
            }
            else if (HiB >= 0xB0 && HiB <= 0xF7)
            {
                //汉字从0xB040--0xF7FE
                if (LoB > 0xFE || LoB == 0x7F || LoB < 0x40)
                {
                    //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                    return false;
                }
            }
            else if (HiB > 0xF7)
            {
                //汉字从0xF840--0xFEA0
                if (HiB >= 0xFF)
                {
                    //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                    return false;
                }
                if (LoB > 0xA0 || LoB == 0x7F || LoB < 0x40)
                {
                    //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                    return false;
                }
                //20111020 20130530 kps3.0中也不可以使用
                if (HiB == 0xFE && LoB >= 0x50)
                {
                    //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                    //Log("out of range : GBK 0xFE50");
                    return false;
                }
            }
        }
        else
        {
            //ASCLL 可见字符范围0x20－0x7E 0x0A 分割符
            unsigned char HiB = (unsigned char) dataBuf[i];
            if ((HiB < 0x20 || HiB > 0x7E) && HiB != 0x0A && HiB != 0x09)
            {
                char buf[20];
                memset(buf, 0, 20);
                sprintf(buf, "%02x", HiB);
                string logStr("填写的发票有非法字符： ");
                logStr.append(buf);
                //LOG4CXX_FATAL(LogUtil::kpLogger, logStr);
                return false;
            }
        }
        i++;
    }
    return res;
}
//截取子串(传入UTF8，按GBK截取字串，传出UTF8)
//len截取子串的长度

string Tools::SubChStr(string str, int len, bool inputGBK, bool outputGBK)
{
    string res = str;
    if (!inputGBK)
    {
        //先转为GBK
        res = UTF82GBK(res);
    }
    if (res.length() <= len)
    {
        if (!outputGBK)
        {
            //转回UTF8
            res = GBK2UTF8(res);
        }
        return res;
    }
    //对中文进行处理
    if ((unsigned char) res[len - 1] <= 0xA0)
    {
        res = res.substr(0, len);
        if (!outputGBK)
        {
            //转回UTF8
            res = GBK2UTF8(res);
        }
        return res;
    }
    int i = len;
    int j = 0;
    while ((unsigned char) res[i - 1] > 0xA0)
    {
        i--;
        j++;
        if (i <= 1)
            break;
    }
    if (j % 2 == 1)
    {
        res = res.substr(0, len - 1);
    }
    else
    {
        res = res.substr(0, len);
    }
    if (!outputGBK)
    {
        //转回UTF8
        res = GBK2UTF8(res);
    }
    return res;

}
//是否全为数字

bool Tools::IsDigit(string str)
{
    for (int i = 0; i < str.length(); ++i)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return false;
        }
    }
    return true;
}
//trim string

string Tools::Trim(const string str)
{
    string data(str);
    data.erase(0, data.find_first_not_of("\r\t\n "));
    data.erase(data.find_last_not_of("\r\t\n ") + 1);
    return data;
}
//格式化整数 FormatFloat(1, 3) = "001"

string Tools::FormatInt(int value, int len)
{
    char buf[100];
    memset(buf, 0, 100);
    //string format = "%0" + Int2Str(len) + "d";
    sprintf(buf, "%0*d", len, value);
    return string(buf);
}
//给字符串以空格增加到指定长度
//aNum 字符串指定的长度
//在末尾添加

string Tools::Addblank(string str, int size)
{
    if (size < str.length())
    {
        return str;
    }
    string blank(size - str.length(), ' ');
    return str + blank;
}
//将str中的字符，逐字写入vs

void Tools::StringToWords(const string str, vector<string> &vs)
{
    //先将UTF-8转为GBK
    /*
    char buf[200];
    memset(buf, 0, 200);
    ConvertCharSet("GBK", "UTF-8", (char *)str.c_str(), str.length(), buf, 200);
    string data(buf);
     */
    string data = UTF82GBK(str);
    //按GBK汉字来解析 一个汉字占2个字节
    while (data.length() > 0)
    {
        //GBK汉字 占2个字节
        if ((unsigned char) data[0] > 0x80)
        {
            vs.push_back(data.substr(0, 2));
            data = data.erase(0, 2);
        }//单字节
        else
        {
            vs.push_back(data.substr(0, 1));
            data = data.erase(0, 1);
        }
    }
}
//中英文混排字符个数统计和编码 传入的str为GBK

int Tools::ChrHZCount(string str, string &last)
{
    //先将UTF-8转为GBK
    //char buf[200];
    //memset(buf, 0, 200);
    //ConvertCharSet("GBK", "UTF-8", (char *)str.c_str(), str.length(), buf, 200);
    //str= buf;

    int result = 0;
    int i = 0;
    int hzcount = 0, ChrCount = 0;
    char *chstr = (char *) str.c_str();
    while (chstr[i] != 0x00)
    {
        //while ((unsigned char)str[i] > 0xA0) {
        if ((unsigned char) chstr[i] > 0xA0)
        {
            hzcount++;
            last = last + "@";
        }
        else
        {
            ChrCount++;
            last = last + "1";
        }
        i++;
    }
    result = ChrCount + hzcount / 2;
    Replace(last, "@@", "0");
    return result;
}
//字符串替换

string Tools::Replace(string src, string searchStr, string replaceStr)
{
    string::size_type pos = 0;
    while ((pos = src.find(searchStr, pos)) != string::npos)
    {
        src.replace(pos, searchStr.size(), replaceStr);
        //pos++;
        pos += replaceStr.size();
    }
    return src;
}
//将某一字符替换为传入的字符串	index从0开始

string Tools::Replace(string src, int index, string replaceStr)
{
    string data(src);
    data.replace(index, 1, replaceStr);
    return data;
}

//字符串中如果有回车换成空格

string Tools::DealReturnStr(string aStr, bool isRepaired)
{
    /*
    char ab = '\n';
    for (int i = aStr.length(); i > 0; i--)
    {
if (aStr[i] == ab)
{
  aStr[i] = ' ';
}
    }
    return aStr;
     */
    string data = aStr;
    if (!isRepaired)
    {
        data = Replace(data, "\n", "￠§");
    }
    else
    {
        data = Replace(data, "￠§", "\n");
    }
    return data;
}
//对于SQL语句,对'号进行处理  postgresql    //

string Tools::DealSqlStr(string str)
{
    string data = str;
    data = Replace(data, "'", "''");
    data = Replace(data, "\\", "\\\\");
    return data;
}
//GBK转UTF8

string Tools::GBK2UTF8(const string str, int len)
{
    /*
    string data(str);
    char *buf = new char[len];
    memset(buf, 0, len);
    if (ConvertCharSet("UTF-8", "GBK", (char *)str.c_str(), str.length(), buf, len) == 0)
    {
            data = buf;
    }
    else
    {
            //
    }
    return data;
     */
    if (len == 0)
        len = 3 * str.length() + 1;
    return EncodingConvert(str, "GBK", "UTF-8", len);
}
//编码转换

string Tools::EncodingConvert(const string str, string fromEncoding, string toEncoding, int len)
{
    string data = str;
    char *tempStr = new char[data.length()];
    memcpy(tempStr, data.c_str(), data.length());
    char *buf = new char[len];
    memset(buf, 0, len);
    if (ConvertCharSet(toEncoding.c_str(), fromEncoding.c_str(), tempStr, data.length(), buf, len) == 0)
    {
        data = buf;
    }
    else
    {
        //
    }
    delete [] buf;
    delete [] tempStr;
    return data;
}
//UTF8转GBK

string Tools::UTF82GBK(const string str, int len)
{
    if (len == 0)
        len = str.length() + 1;
    return EncodingConvert(str, "UTF-8", "GBK", len);
}

string Tools::UTF82GB18030(const string str, int len)
{
    if (len == 0)
        len = str.length() + 1;
    return EncodingConvert(str, "UTF-8", "GB18030", len);
}

//字符串以16进制输出

string Tools::ToHexString(const string &str)
{
    char buf[2];
    char *buffer = new char[str.length()*2 + 1];
    memset(buffer, 0, str.length()*2 + 1);
    for (int i = 0; i < str.length(); ++i)
    {
        sprintf(buf, "%02x", (unsigned char) str[i]);
        buffer[2 * i] = buf[0];
        buffer[2 * i + 1] = buf[1];
    }
    string data(buffer);
    delete [] buffer;
    return data;
}

string Tools::BytesToHex(char* byteArray, unsigned long len)
{
    char *buffer = new char[len * 2 + 1];
    memset(buffer, 0, len * 2 + 1);
    for (unsigned long i = 0L; i < len; ++i)
    {
        sprintf(buffer + i * 2, "%02x", byteArray[i]);
    }
    buffer[len * 2] = '\0';
    string data(buffer);
    delete [] buffer;
    return data;
}

string Tools::Bytes2Hex(unsigned char* byteArray)
{
    char buf[2];
    char *buffer = new char[sizeof (byteArray) * 2 + 1];
    memset(buffer, 0, sizeof (byteArray) * 2 + 1);
    for (int i = 0; i < sizeof (byteArray); ++i)
    {
        sprintf(buf, "%02x", byteArray[i]);
        buffer[2 * i] = buf[0];
        buffer[2 * i + 1] = buf[1];
    }
    buffer[sizeof (byteArray) * 2] = '\0';
    string data(buffer);
    delete [] buffer;
    return data;
}

unsigned char* Tools::Hex2Bytes(const string& hexStr)
{
    if (hexStr.length() % 2)
    {
        return NULL;
    }

    unsigned char* byteArray = new unsigned char[hexStr.length() / 2 + 1];
    for (int i = 0; i < hexStr.length() / 2; i++)
    {
        unsigned char prefix, suffix;
        char prefixChar = hexStr[2 * i];
        char suffixChar = hexStr[2 * i + 1];
        if ('0' <= prefixChar && prefixChar <= '9')
        {
            prefix = prefixChar - '0';
        }
        else if ('a' <= prefixChar && prefixChar <= 'f')
        {
            prefix = prefixChar - 'a' + 10;
        }
        else if ('A' <= prefixChar && prefixChar <= 'F')
        {
            prefix = prefixChar - 'A' + 10;
        }

        if ('0' <= suffixChar && suffixChar <= '9')
        {
            suffix = suffixChar - '0';
        }
        else if ('a' <= suffixChar && suffixChar <= 'f')
        {
            suffix = suffixChar - 'a' + 10;
        }
        else if ('A' <= suffixChar && suffixChar <= 'F')
        {
            suffix = suffixChar - 'A' + 10;
        }

        byteArray[i] = (prefix << 4) + suffix;
    }
    byteArray[hexStr.length() / 2] = '\0';
    return byteArray;
}

void Tools::Hex2Bytes2(const string& hexStr, unsigned char* byteArray, int &leng)
{
    //    unsigned char* byteArray = new unsigned char[hexStr.length() / 2 + 1];
    for (int i = 0; i < hexStr.length() / 2; i++)
    {
        unsigned char prefix, suffix;
        char prefixChar = hexStr[2 * i];
        char suffixChar = hexStr[2 * i + 1];
        if ('0' <= prefixChar && prefixChar <= '9')
        {
            prefix = prefixChar - '0';
        }
        else if ('a' <= prefixChar && prefixChar <= 'f')
        {
            prefix = prefixChar - 'a' + 10;
        }
        else if ('A' <= prefixChar && prefixChar <= 'F')
        {
            prefix = prefixChar - 'A' + 10;
        }

        if ('0' <= suffixChar && suffixChar <= '9')
        {
            suffix = suffixChar - '0';
        }
        else if ('a' <= suffixChar && suffixChar <= 'f')
        {
            suffix = suffixChar - 'a' + 10;
        }
        else if ('A' <= suffixChar && suffixChar <= 'F')
        {
            suffix = suffixChar - 'A' + 10;
        }

        byteArray[i] = (prefix << 4) + suffix;
    }
    byteArray[hexStr.length() / 2] = '\0';
    leng = hexStr.length() / 2;

}



//String 转换为 Double

double Tools::Str2Double(const string data)
{
    if (data == "" || data.length() == 0 || data == "NULL")
    {
        return 0.0;
    }
    double _value = 0.0;
    std::istringstream iss(data);
    iss >> _value;
    return _value;
}

//String 转换为Bool

bool Tools::Str2Bool(string data)
{
    bool ret = false;
    data = Tools::StringToLowerCase(data);
    if (data.length() > 0)
    {
        if (data == "true" || data == "1" || data == "是")
        {
            ret = true;
        }
    }
    return ret;

}

//小写字符转换为大写

string Tools::StringToUpperCase(string data)
{
    string data2 = data;
    for (int i = 0; i < data2.length(); i++)
    {
        if (data2[i] >= 'a' && data2[i] <= 'z')
        {
            data2[i] = data2[i] - 32;
        }
    }
    return data2;
}
//大写字符转换为小写

string Tools::StringToLowerCase(string data)
{
    string data2 = data;
    for (int i = 0; i < data2.length(); i++)
    {
        if (data2[i] >= 'A' && data2[i] <= 'Z')
        {
            data2[i] = data2[i] + 32;
        }
    }
    return data2;
}

// String转换为DateTime

time_t Tools::StringToDateTime(string data)
{
    if (data == "" || data.length() == 0)
    {
        return time(NULL); /*获取time_t类型当前时间*/
    }
    char ch1[100];
    strcpy(ch1, data.c_str());
    time_t dt;
    dt = stringTOtime_t(ch1);
    return dt;
}






//这里的time_t可以传入一个unsigned int类型的参数，其表示的含义是1900.1.1 0:0:0开始计时的秒数，返回字符串结构如“2011-08-05 00:00:00”。

string Tools::NowTimeToString(time_t iTimeStamp)
{
    tm *pTmp = localtime(&iTimeStamp);
    if (pTmp == NULL)
    {
        return "1900-01-01 00:00:00";
    }
    std::stringstream year, mon, day, h, m, s;
    string timeString;
    year << pTmp->tm_year + 1900;
    mon << pTmp->tm_mon + 1;
    day << pTmp->tm_mday;
    h << pTmp->tm_hour;
    m << pTmp->tm_min;
    s << pTmp->tm_sec;
    string yearStr, monStr, dayStr, hStr, mStr, sStr;

    yearStr = year.str();
    if (mon.str().length() < 2)
    {
        monStr = "0" + mon.str();
    }
    else
    {
        monStr = mon.str();
    }
    if (day.str().length() < 2)
    {
        dayStr = "0" + day.str();
    }
    else
    {
        dayStr = day.str();
    }
    if (h.str().length() < 2)
    {
        hStr = "0" + h.str();
    }
    else
    {
        hStr = h.str();
    }
    if (m.str().length() < 2)
    {
        mStr = "0" + m.str();
    }
    else
    {
        mStr = m.str();
    }

    if (s.str().length() < 2)
    {
        sStr = "0" + s.str();
    }
    else
    {
        sStr = s.str();
    }

    timeString = yearStr + "-" + monStr + "-" + dayStr + " " + hStr + ":" + mStr + ":" + sStr;
    return timeString;
}

//返回字符串结构如“20110805000000”。

string Tools::NowTimeToString1(time_t iTimeStamp)
{
    tm *pTmp = localtime(&iTimeStamp);
    if (pTmp == NULL)
    {
        return "1900-01-01 00:00:00";
    }
    std::stringstream year, mon, day, h, m, s;
    string timeString;
    year << pTmp->tm_year + 1900;
    mon << pTmp->tm_mon + 1;
    day << pTmp->tm_mday;
    h << pTmp->tm_hour;
    m << pTmp->tm_min;
    s << pTmp->tm_sec;
    string yearStr, monStr, dayStr, hStr, mStr, sStr;

    yearStr = year.str();
    if (mon.str().length() < 2)
    {
        monStr = "0" + mon.str();
    }
    else
    {
        monStr = mon.str();
    }
    if (day.str().length() < 2)
    {
        dayStr = "0" + day.str();
    }
    else
    {
        dayStr = day.str();
    }
    if (h.str().length() < 2)
    {
        hStr = "0" + h.str();
    }
    else
    {
        hStr = h.str();
    }
    if (m.str().length() < 2)
    {
        mStr = "0" + m.str();
    }
    else
    {
        mStr = m.str();
    }

    if (s.str().length() < 2)
    {
        sStr = "0" + s.str();
    }
    else
    {
        sStr = s.str();
    }
    timeString = yearStr + monStr + dayStr + hStr + mStr + sStr;
    return timeString;
}

time_t Tools::stringTOtime_t(const char * szTime)
{
    struct tm tm1;
    time_t time1;
    int i = 0;
    string tmpTime = szTime;
    if (tmpTime.find_first_of('-') != -1)
    {
        sscanf(szTime, "%d-%d-%d %d:%d:%d",
               &(tm1.tm_year),
               &(tm1.tm_mon),
               &(tm1.tm_mday),
               &(tm1.tm_hour),
               &(tm1.tm_min),
               &(tm1.tm_sec)
               );
    }
    else
    {
        sscanf(szTime, "%d%d%d%d%d%d%d%d",
               &(tm1.tm_year),
               &(tm1.tm_mon),
               &(tm1.tm_mday),
               &(tm1.tm_hour),
               &(tm1.tm_min),
               &(tm1.tm_sec),
               &(tm1.tm_wday),
               &(tm1.tm_yday));
    }
    //    cout << "i= " << i << endl;

    tm1.tm_year -= 1900;
    tm1.tm_mon--;
    tm1.tm_isdst = -1;

    time1 = mktime(&tm1);
    // cout << endl << "time= " << time1 << endl;
    return time1;

}


//转换FPHM为前面加0，补齐8位

string Tools::FPHMTo8Wei(string strValue)
{
    string value;
    int strLength = strValue.length();
    if (strLength < 8)
    {
        for (int i = 0; i < 8 - strLength; i++)
        {
            value += "0";
        }
    }
    value += strValue;
    return value;
}

//转换FPHM为前面加0，补齐8位

string Tools::FPHMTo8Wei(int intValue)
{
    string value;
    std::stringstream ss;
    ss << intValue;
    value = ss.str();
    string egihtValue = FPHMTo8Wei(value);
    return egihtValue;
}

//Convert String to length Number Format ,
//Example，  transform int 11 to String 00011,
//string tarStr = formatLengthStr(11,5) 

string Tools::formatLengthStr(int intValue, int length)
{
    string value;
    std::stringstream ss;
    ss << intValue;
    value = ss.str();
    string lengthValue = formatLengthStr(value, length);
    return lengthValue;
}

string Tools::formatLengthStr(string strValue, int length)
{
    string value;
    int strLength = strValue.length();
    if (strLength < length)
    {
        for (int i = 0; i < length - strLength; i++)
        {
            value += "0";
        }
    }
    value += strValue;
    return value;
}


//根据营业税标识，判断主要发票类型

void Tools::GetYYSBZ(Fpxx fpxx)
{
    string yysbz = fpxx.yysbz;
    if (yysbz == "" || yysbz.length() < 10)
    {
        return;
    }
    switch (yysbz[2])
    {
    case '1':
        fpxx.zyfpLx = ZYLX_XT_YCL;
        break;
    case '2':
        fpxx.zyfpLx = ZYLX_XT_CCP;
        break;
    case '3':
        fpxx.zyfpLx = ZYLX_SNY;
        break;
    default:
        break;
    }

    //判断新旧机动车
    switch (yysbz[4])
    {
    case '1':
        fpxx.isNewJdcfp = false;
        break;
    default:
        fpxx.isNewJdcfp = true;
        break;
    }

    //判断农产品
    switch (yysbz[5])
    {
    case '1':
        fpxx.zyfpLx = ZYLX_NCP_XS;
        break;
    case '2':
        fpxx.zyfpLx = ZYLX_NCP_SG;
        break;
    default:
        break;
    }
    double zfbz = true;

    //空白作废
    double fpje = Tools::Str2Double(fpxx.je);
    if (fpje == 0 && zfbz == true)
    {
        fpxx.isBlankWaste = true;
    }
    else
    {
        fpxx.isBlankWaste = false;
    }
}

// 转换发票类型为回传发票类型
//TODO FPLX 未正确设置

string Tools::FPLXToDBType(FPLX fplx)
{
    switch (fplx)
    {
    case ZYFP:
        return "s";
    case PTFP:
        return "c";
    case HYFP:
        return "f";
    case JDCFP:
        return "j";
    case DZFP:
        return "p";
    default:
        return "s";
    }
}



//转换发票类型为回传发票类型

string Tools::FPLXToDBType(string fplx)
{
    if (fplx == "专用发票")
        return "s";
    else if (fplx == "普通发票")
        return "c";
    else if (fplx == "货物运输业增值税专用发票")
        return "f";
    else if (fplx == "机动车销售统一发票")
        return "j";
    else if (fplx == "增值税普通发票(电子)")
        return "p";
    else
        return "s";
}


//转换回传发票类型为发票类型

string Tools::DBTypeToFPLX(char dbType)
{
    switch (dbType)
    {
    case 's':
        return "专用发票";
    case 'c':
        return "普通发票";
    case 'f':
        return "货物运输业增值税专用发票";
    case 'j':
        return "机动车销售统一发票";
    case 'p':
        return "增值税普通发票(电子)";
    default:
        return "专用发票";
    }
}

//TODO

int Tools::FPLXToCardType(string fplx)
{
    if (fplx == "专用发票" || fplx == "s")
    {
        return (int) ZYFP;
    }
    else if (fplx == "普通发票" || fplx == "c")
    {
        return (int) PTFP;
    }
    else if (fplx == "货物运输业增值税专用发票" || fplx == "f")
    {
        return (int) HYFP;
    }
    else if (fplx == "机动车销售统一发票" || fplx == "j")
    {
        return (int) JDCFP;
    }
    else if (fplx == "增值税普通发票(电子)" || fplx == "p")
    {
        return (int) DZFP;
    }
    else
    {
        return (int) ZYFP;
    }
}
//generate random number,
//format as 2015092216582012457

string Tools::GetStatusCode()
{
    //2015092216582012457
    time_t ta = time(NULL);
    string strTime = Tools::NowTimeToString1(ta);
    strTime = Tools::Replace(strTime, ":", "");
    strTime = Tools::Replace(strTime, "-", "");
    strTime = Tools::Replace(strTime, " ", "");
    srand((unsigned) time(NULL));
    int randNum = rand() % 10000;
    string randNumStr = Tools::formatLengthStr(randNum, 4);
    strTime += randNumStr;

    return strTime;
}


//解析错误号

int Tools::GetReturnErrCode(string errcode)
{
    int ret = -1;
    if (errcode.length() > 0)
    {
        int index = errcode.find_last_of('_');
        if (index == -1)
        {
            if (errcode == "0000")
            {
                return 0;
            }
            else
            {
                int retV = -1;
                retV = Tools::Str2Int(errcode);
                return retV;
            }
        }
        else
        {
            //                    string[] list = errcode.Split(new char[] { '_' });
            //                    ret = Int32.Parse(list[1]);
            vector<string> vec = Split(errcode, "_");
            if (vec.size() < 2)
            {
                return -1;
            }
            ret = Str2Int(vec[1]);
        }
    }
    return ret;

}

void Tools::ReplaceAll(string&s1, const string&s2, const string&s3)
{
    string::size_type pos = 0;
    string::size_type a = s2.size();
    string::size_type b = s3.size();
    while ((pos = s1.find(s2, pos)) != string::npos)
    {
        s1.replace(pos, a, s3);
        pos += b;
    }
}

void Tools::ReplaceAll_SQL(string&s1, const string&s2, const string&s3)
{
    string::size_type pos = 0;
    string::size_type a = s2.size();
    string::size_type b = s3.size();
    int slen = s1.size();
    while ((pos = s1.find(s2, pos)) != string::npos)
    {
        if (s3 != "NULL" && pos + a < slen && s1[pos + a] != '\'')
        {
            pos += a;
            continue;
        }
        s1.replace(pos, a, s3);
        slen = s1.size();
        pos += b;
    }
}

vector<string> Tools::split(string str, string separator)
{
    vector<string> result;
    int cutAt;
    while ((cutAt = str.find_first_of(separator)) != str.npos)
    {
        if (cutAt > 0)
        {
            result.push_back(str.substr(0, cutAt));
        }
        str = str.substr(cutAt + separator.length());
    }
    if (str.length() > 0)
    {
        result.push_back(str);
    }
    return result;
}

string Tools::FromBase64String(string str)
{
    string outData = "";
    char * input = (char *) str.c_str();
    int rlen = 3 * str.size();
    char * result = new char[rlen];

    Base64Utils::Base64_Decode(input, (unsigned char *) result, &rlen);
    outData = string(result, rlen);
    if (result != 0)
    {
        delete result;
        result = 0;
    }
    return outData;
}

string Tools::GetString(string data)
{
    if (data == "")
    {
        return "";
    }
    return EncodingConvert(data, "UTF-8", "GBK", 3 * data.size());
}

string Tools::GetString(string data, int index, int len)
{
    if (data == "")
    {
        return "";
    }

    string tmp = data.substr(index, len);
    return EncodingConvert(tmp, "GBK", "UTF-8", 3 * tmp.size());
    ;
}

string Tools::AES_Erypt(string data, byte key[16], byte vec[16])
{
    //unsigned char tmp[10240] = {0};
    char * tmp = new char[3000000];
    memset(tmp, 0, sizeof (tmp));
    string OutData = "";
    uint32 tmp_len = 0;
    string tmpMW = Tools::GetString(data);
    aes_encrypt_cbc(key, vec, (unsigned char*) (tmpMW.c_str()), tmpMW.length(), (unsigned char*) tmp, &tmp_len);

    OutData = string((char*) tmp, tmp_len);
    if (tmp != 0)
    {
        delete tmp;
        tmp = 0;
    }
    return OutData;
}

string Tools::AES_Decrypt(string data, byte key[16], byte vec[16], string flag)
{
    unsigned char tmp[10240] = {0};
    string OutData = "";
    uint32 tmp_len = 0;
    string tmpMW = Tools::GetString(data);
    aes_decrypt_cbc(key, vec, (unsigned char*) (tmpMW.c_str()), tmpMW.length(), tmp, &tmp_len, flag);

    OutData = string((char*) tmp, tmp_len);
    return OutData;
}


/// <summary>
/// 从备注中获得通知单号，用于红字发票校验，类型0-普票，1-专票，2-货运发票；3-机动车；4-电子发票，如果格式正确专票和货运发票返回通知单号串，普票和机动车返回发票代码和发票号码的合并串
/// </summary>
/// <param name="Notes"></param>
/// <param name="type"></param>
/// <param name="separator"></param>
/// <returns></returns>

string Tools::GetInfo(string data, int type, string separator)
{
    string Result = ""; //保存结果
    string Store = ""; //保存发票代码

    if (data == "") //传入为空
    {
        return Result;
    }

    if (type == 1 || type == 2) //专用发票或货运发票
    {
        string prefix = (type == 1 ? PREFIX_TZDH_BZ : PREFIX_HYTZDH_BZ);

        string fpbz = data;
        vector<string> buf = split(fpbz, "\r\n");
        for (vector<string>::iterator begin = buf.begin(), end = buf.end(); begin != end; ++begin)
        {
            string str = *begin;
            int pos1 = str.find(prefix);
            if (pos1 < 0)
            {
                continue;
            }
            string temptzdh = str.substr(pos1 + prefix.size());
            if (temptzdh == "")
            {
                continue;
            }
            else
            {
                if (temptzdh.size() > 16)
                {
                    temptzdh = temptzdh.substr(0, 16);
                }
                Result = temptzdh;
                break;
            }
        }
        return Result;
    }
    else if (type == 0 || type == 3 || type == 4) //普通发票或机动车发票或电子发票
    {

        string fpbz = data;
        string lzfpdm = "";
        string lzfphm = "";
        vector<string> buf = split(fpbz, "\r\n");
        for (vector<string>::iterator begin = buf.begin(), end = buf.end(); begin != end; ++begin)
        {
            string str = *begin;
            int pos1 = str.find(PREFIX_FPDM_BZ);
            int pos2 = str.find(PREFIX_FPHM_BZ);
            if (pos1 < 0 || pos2 < 0 || pos1 >= pos2)
            {
                continue;
            }
            string templzfpdm = str.substr(pos1 + PREFIX_FPDM_BZ.size(), pos2 - pos1 - PREFIX_FPDM_BZ.size());
            string templzfphm = str.substr(pos2 + PREFIX_FPHM_BZ.size());
            if ("" == templzfpdm || "" == templzfphm)
            {
                continue;
            }
            else
            {
                if (templzfphm.size() > 8)
                {
                    templzfphm = templzfphm.substr(0, 8);
                }
                lzfpdm = templzfpdm;
                lzfphm = templzfphm;
                Result = lzfpdm + lzfphm;
                if (separator != "")
                {
                    Result = GetPreDigital(lzfpdm) + separator + GetPreDigital(lzfphm);
                }
                break;
            }
        }
        return Result;
    }
    else
    {
        return Result;
    }
}

/// <summary>
/// 获取data前面的数字
/// </summary>
/// <param name="data"></param>
/// <returns></returns>

string Tools::GetPreDigital(string data)
{
    string sb = "";
    int len = data.size();
    for (int i = 0; i < len; ++i)
    {
        if (isdigit(data[i]) == true)
        {
            sb += data[i];
        }
        else
        {
            break;
        }
    }
    return sb;
}

string Tools::GetBytes(string data)
{
    string outData = Tools::GetString(data);
    return data;
}

string Tools::Add(string data1, string data2)
{
    double a = Tools::Str2Double(data1);
    double b = Tools::Str2Double(data2);
    double ab = a + b;
    string abStr = Tools::DoubleToString(ab);
    return abStr;
}

char * Tools::Copy(char * src, int index, char * buf, int len)
{
    if (src == 0 || buf == 0 || len == 0)
    {
        return 0;
    }

    char * start = src + index;
    memcpy(buf, start, len);
    return buf;
}

void Tools::GetDKQYFromInvNotes(string fpbz, string & dkqysh, string & dkqymc)
{
    string result = "0001";
    dkqysh = "";
    dkqymc = "";
    vector<string> buf = split(fpbz, "\r\n");
    for (vector<string>::iterator begin = buf.begin(), end = buf.end(); begin != end; ++begin)
    {
        string str = *begin;
        int pos1 = str.find(PREFIX_DKQYSH);
        int pos2 = str.find(PREFIX_DKQYMC);
        if (pos1 < 0 || pos2 < 0 || pos1 >= pos2)
        {
            continue;
        }
        string tempsh = str.substr(pos1 + PREFIX_DKQYSH.size(), pos2 - pos1 - PREFIX_DKQYSH.size());
        string tempmc = str.substr(pos2 + PREFIX_DKQYMC.size());
        if ("" == tempsh || tempmc == "")
        {
            continue;
        }
        else
        {
            dkqysh = tempsh;
            dkqymc = tempmc;
            result = "0000";
            break;
        }
    }
}

/// <summary>
/// 截取Decimal类型字段的最大长度
/// </summary>
/// <param name="s">原始数据</param>
/// <param name="len">数据最大长度</param>
/// <param name="addTail">当len大于s的字节长度值时，尾部是否补加空格，直到字节长度达到len为止</param>
/// <returns></returns>

string Tools::GetSubDecimal(string data, int len, bool flag)
{
    try
    {
        if (data == "")
        {
            if (!flag)
                return "";
            else
                return string(len, ' ');
        }

        string value = data;
        int value_len = data.size();
        if (value_len > len)
        {
            int idx = value.find('.');
            if (idx >= 0)
            {
                short dec = value_len - (idx + 1) - (value_len - len);
                if (dec >= -1)
                {
                    double tmp = Tools::Str2Double(data);
                    dec = dec < 0 ? 0 : dec;
                    tmp = Round(tmp, dec);
                    return Tools::DoubleToString(tmp);
                }
                else
                    return "";
            }
            else
            {
                if (value_len > len)
                    return "";
                else
                {
                    if (flag)
                    {
                        value += string(len - value_len, ' ');
                        return value;
                    }
                    else
                        return value;
                }
            }
        }
        else
        {
            if (flag)
            {
                value += string(len - value_len, ' ');
                return value;
            }
            else
                return value;
        }
    }
    catch (exception)
    {
        return "";
    }
    return data;
}

string Tools::MultiplyStr(string data1, string data2, int presicise)
{
    double a = Tools::Str2Double(data1);
    double b = Tools::Str2Double(data2);
    double axb = Round(a * b, presicise);
    string axbStr = Tools::DoubleToString(axb);
    return axbStr;
}

string Tools::DivideStr(string data1, string data2, int presicise)
{
    double a = Tools::Str2Double(data1);
    double b = Tools::Str2Double(data2);
    double abstr = Round(a / b, presicise);
    return Tools::DoubleToString(abstr);
}

string Tools::Multiply(string data1, string data2, int presicise)
{
    return MultiplyStr(data1, data2, presicise);
}

string Tools::FormatDateTime(time_t t)
{
    struct tm pTm = Tools::time_t2tm(t);
    string dateStr = FormatInt(pTm.tm_year, 4) + "-" + FormatInt(pTm.tm_mon, 2) + "-" + FormatInt(pTm.tm_mday, 2)
            + " " + FormatInt(pTm.tm_hour, 2) + ":" + FormatInt(pTm.tm_min, 2) + ":" + FormatInt(pTm.tm_sec, 2);
    return dateStr;
}

string Tools::ToBase64String(string data)
{
    string outData = "";
    char * input = (char *) data.c_str();
    int rlen = data.size();
    char * result = new char[rlen * 2];

    Base64Utils::Base64_Encode((unsigned char *) input, rlen, result);
    outData = result;
    if (result != 0)
    {
        delete result;
        result = 0;
    }
    return outData;
}

double Tools::Round(double dVal, short iPlaces)
{
    double dRetval;
    double dMod = 0.0000001;
    if (dVal < 0.0) dMod = -0.0000001;
    dRetval = dVal;
    dRetval += (5.0 / pow(10.0, iPlaces + 1.0));
    dRetval *= pow(10.0, iPlaces);
    dRetval = floor(dRetval + dMod);
    dRetval /= pow(10.0, iPlaces);
    return (dRetval);
}

double Tools::Round(string dVal, short iPlaces)
{
    double data = Str2Double(dVal);
    return Round(data, iPlaces);
}

string Tools::DoubleToString(double data)
{
    char Buffer[30] = {0};
    gcvt(data, 20, Buffer);
    return Buffer;
}

string Tools::Serialize(vector<ClearCardInfo> clearList)
{
    XMLDocument xmlDoc;
    XMLDeclaration *xmlDec = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\"");
    xmlDoc.InsertEndChild(xmlDec);

    XMLElement* rootElement = xmlDoc.NewElement("ClearCardInfos");
    xmlDoc.InsertEndChild(rootElement);

    for (vector<ClearCardInfo>::iterator begin = clearList.begin(), end = clearList.end(); begin != end; ++begin)
    {
        ClearCardInfo cardInfo = *begin;

        XMLElement* ClearInfo = xmlDoc.NewElement("ClearInfo");
        rootElement->InsertEndChild(ClearInfo);

        XMLElement * InvKind = xmlDoc.NewElement("InvKind");
        InvKind->SetText(cardInfo.InvKind);
        ClearInfo->InsertEndChild(InvKind);

        XMLElement * InvPeriod = xmlDoc.NewElement("InvPeriod");
        InvPeriod->SetText(cardInfo.InvPeriod);
        ClearInfo->InsertEndChild(InvPeriod);

        XMLElement* CSTime = xmlDoc.NewElement("CSTime");
        CSTime->SetText(cardInfo.CSTime.c_str());
        ClearInfo->InsertEndChild(CSTime);

    }

    XMLPrinter xmlPrinter;
    xmlDoc.Accept(&xmlPrinter);
    string xmlStr = xmlPrinter.CStr();
    string outData = Tools::UTF82GB18030(xmlPrinter.CStr());
    string Base64 = Tools::ToBase64String(outData);
    return Base64;
}

string Tools::Serialize(ClearCardInfo clearInfo)
{
    XMLDocument xmlDoc;
    XMLDeclaration *xmlDec = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\"");
    xmlDoc.InsertEndChild(xmlDec);

    XMLElement* rootElement = xmlDoc.NewElement("ClearInfo");
    xmlDoc.InsertEndChild(rootElement);

    XMLElement * InvKind = xmlDoc.NewElement("InvKind");
    InvKind->SetText(clearInfo.InvKind);
    rootElement->InsertEndChild(InvKind);

    XMLElement * InvPeriod = xmlDoc.NewElement("InvPeriod");
    InvPeriod->SetText(clearInfo.InvPeriod);
    rootElement->InsertEndChild(InvPeriod);

    XMLElement* CSTime = xmlDoc.NewElement("CSTime");
    CSTime->SetText(clearInfo.CSTime.c_str());
    rootElement->InsertEndChild(CSTime);


    XMLPrinter xmlPrinter;
    xmlDoc.Accept(&xmlPrinter);
    string xmlStr = xmlPrinter.CStr();
    string outData = Tools::UTF82GB18030(xmlPrinter.CStr());
    string Base64 = Tools::ToBase64String(outData);
    return Base64;
}

string Tools::Serialize(CSDateInfo dateInfo)
{
    XMLDocument xmlDoc;
    XMLDeclaration *xmlDec = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\"");
    xmlDoc.InsertEndChild(xmlDec);

    XMLElement* rootElement = xmlDoc.NewElement("DateInfo");
    xmlDoc.InsertEndChild(rootElement);

    XMLElement * LastRepDate = xmlDoc.NewElement("LastRepDate");
    LastRepDate->SetText(dateInfo.LastRepDate.c_str());
    rootElement->InsertEndChild(LastRepDate);

    XMLElement * LockedDate = xmlDoc.NewElement("LockedDate");
    LockedDate->SetText(dateInfo.LockedDate.c_str());
    rootElement->InsertEndChild(LockedDate);

    XMLElement* NextRepDate = xmlDoc.NewElement("NextRepDate");
    NextRepDate->SetText(dateInfo.NextRepDate.c_str());
    rootElement->InsertEndChild(NextRepDate);

    XMLPrinter xmlPrinter;
    xmlDoc.Accept(&xmlPrinter);
    string xmlStr = xmlPrinter.CStr();
    string outData = Tools::UTF82GB18030(xmlPrinter.CStr());
    string Base64 = Tools::ToBase64String(outData);
    return Base64;
}

string Tools::Serialize(InvVolumeApp ivvAppInfo)
{
    XMLDocument xmlDoc;
    XMLDeclaration *xmlDec = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\"");
    xmlDoc.InsertEndChild(xmlDec);

    XMLElement* rootElement = xmlDoc.NewElement("ivvAppInfo");
    xmlDoc.InsertEndChild(rootElement);

    //InvType
    XMLElement *InvType = xmlDoc.NewElement("InvType");
    InvType->SetText(ivvAppInfo.InvType);
    rootElement->InsertEndChild(InvType);

    //InvLimit
    XMLElement *InvLimit = xmlDoc.NewElement("InvLimit");
    InvLimit->SetText(Long2Str(ivvAppInfo.InvLimit).c_str());
    rootElement->InsertEndChild(InvLimit);

    //BuyDate
    XMLElement *BuyDate = xmlDoc.NewElement("BuyDate");
    BuyDate->SetText(Tools::Time_t2String(ivvAppInfo.BuyDate).c_str());
    rootElement->InsertEndChild(BuyDate);

    //SaledDate
    XMLElement *SaledDate = xmlDoc.NewElement("SaledDate");
    SaledDate->SetText(Tools::Time_t2String(ivvAppInfo.SaledDate).c_str());
    rootElement->InsertEndChild(SaledDate);

    //TypeCode
    XMLElement *typeCode = xmlDoc.NewElement("TypeCode");
    typeCode->SetText(ivvAppInfo.TypeCode.c_str());
    rootElement->InsertEndChild(typeCode);

    //HeadCode
    XMLElement *HeadCode = xmlDoc.NewElement("HeadCode");
    HeadCode->SetText(ivvAppInfo.HeadCode);
    rootElement->InsertEndChild(HeadCode);

    //Number
    XMLElement *Number = xmlDoc.NewElement("Number");
    Number->SetText(ivvAppInfo.Number);
    rootElement->InsertEndChild(Number);

    //BuyNumber
    XMLElement *BuyNumber = xmlDoc.NewElement("BuyNumber");
    BuyNumber->SetText(ivvAppInfo.BuyNumber);
    rootElement->InsertEndChild(BuyNumber);

    //Status
    XMLElement *Status = xmlDoc.NewElement("HeadCode");
    Status->SetText(ivvAppInfo.HeadCode);
    rootElement->InsertEndChild(HeadCode);

    //ClientNumber
    XMLElement *ClientNumber = xmlDoc.NewElement("ClientNumber");
    ClientNumber->SetText(ivvAppInfo.ClientNumber);
    rootElement->InsertEndChild(ClientNumber);

    XMLPrinter xmlPrinter;
    xmlDoc.Accept(&xmlPrinter);
    string xmlStr = xmlPrinter.CStr();
    string outData = Tools::UTF82GB18030(xmlPrinter.CStr());
    string Base64 = Tools::ToBase64String(outData);
    return Base64;
}

string Tools::Serialize(InvDetail invDetailInfo)
{
    XMLDocument xmlDoc;
    XMLDeclaration *xmlDec = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\"");
    xmlDoc.InsertEndChild(xmlDec);

    XMLElement* rootElement = xmlDoc.NewElement("InvDetailInfo");
    xmlDoc.InsertEndChild(rootElement);

    //Amount
    XMLElement * Amount = xmlDoc.NewElement("Amount");
    Amount->SetText(invDetailInfo.Amount);
    rootElement->InsertEndChild(Amount);

    //BuyTaxCode
    XMLElement * BuyTaxCode = xmlDoc.NewElement("BuyTaxCode");
    BuyTaxCode->SetText(invDetailInfo.BuyTaxCode.c_str());
    rootElement->InsertEndChild(BuyTaxCode);

    //CancelFlag
    XMLElement* CancelFlag = xmlDoc.NewElement("CancelFlag");
    CancelFlag->SetText(invDetailInfo.CancelFlag);
    rootElement->InsertEndChild(CancelFlag);


    //ClientNum
    XMLElement* ClientNum = xmlDoc.NewElement("ClientNum");
    ClientNum->SetText(invDetailInfo.ClientNum);
    rootElement->InsertEndChild(ClientNum);

    //Date
    XMLElement* Date = xmlDoc.NewElement("Date");
    Date->SetText(Tools::Time_t2String(invDetailInfo.Date).c_str());
    rootElement->InsertEndChild(Date);

    //Date
    XMLElement* Index = xmlDoc.NewElement("Index");
    Index->SetText(invDetailInfo.Index.c_str());
    rootElement->InsertEndChild(Index);


    //InvNo
    XMLElement* InvNo = xmlDoc.NewElement("InvNo");
    InvNo->SetText(invDetailInfo.InvNo);
    rootElement->InsertEndChild(InvNo);

    //InvQryNo
    XMLElement* InvQryNo = xmlDoc.NewElement("InvQryNo");
    InvQryNo->SetText((int) invDetailInfo.InvQryNo);
    rootElement->InsertEndChild(InvQryNo);


    //InvRepPeriod
    XMLElement* InvRepPeriod = xmlDoc.NewElement("InvRepPeriod");
    InvRepPeriod->SetText(invDetailInfo.InvRepPeriod);
    rootElement->InsertEndChild(InvRepPeriod);


    //InvSeqNo
    XMLElement* InvSeqNo = xmlDoc.NewElement("InvSeqNo");
    InvSeqNo->SetText(invDetailInfo.InvSeqNo.c_str());
    rootElement->InsertEndChild(InvSeqNo);


    //InvType
    XMLElement* InvType = xmlDoc.NewElement("InvType");
    InvType->SetText(invDetailInfo.InvType);
    rootElement->InsertEndChild(InvType);


    //IsUpload
    XMLElement* IsUpload = xmlDoc.NewElement("IsUpload");
    IsUpload->SetText(invDetailInfo.IsUpload);
    rootElement->InsertEndChild(IsUpload);


    //JYM
    XMLElement* JYM = xmlDoc.NewElement("JYM");
    JYM->SetText(invDetailInfo.JYM.c_str());
    rootElement->InsertEndChild(JYM);


    //MW
    XMLElement* MW = xmlDoc.NewElement("MW");
    MW->SetText(invDetailInfo.MW.c_str());
    rootElement->InsertEndChild(MW);


    //OldInvNo
    XMLElement* OldInvNo = xmlDoc.NewElement("OldInvNo");
    OldInvNo->SetText(invDetailInfo.OldInvNo.c_str());
    rootElement->InsertEndChild(OldInvNo);

    //OldTypeCode
    XMLElement* OldTypeCode = xmlDoc.NewElement("OldTypeCode");
    OldTypeCode->SetText(invDetailInfo.OldTypeCode.c_str());
    rootElement->InsertEndChild(OldTypeCode);


    //SaleTaxCode
    XMLElement* SaleTaxCode = xmlDoc.NewElement("SaleTaxCode");
    SaleTaxCode->SetText(invDetailInfo.SaleTaxCode.c_str());
    rootElement->InsertEndChild(SaleTaxCode);

    //SignBuffer
    XMLElement* SignBuffer = xmlDoc.NewElement("SignBuffer");
    SignBuffer->SetText(invDetailInfo.SignBuffer.c_str());
    rootElement->InsertEndChild(SignBuffer);

    //Tax
    XMLElement* Tax = xmlDoc.NewElement("Tax");
    Tax->SetText(invDetailInfo.Tax);
    rootElement->InsertEndChild(Tax);

    //Tax
    XMLElement* TaxClass = xmlDoc.NewElement("TaxClass");
    TaxClass->SetText(invDetailInfo.TaxClass);
    rootElement->InsertEndChild(TaxClass);

    //TypeCode
    XMLElement* TypeCode = xmlDoc.NewElement("TypeCode");
    TypeCode->SetText(invDetailInfo.TypeCode.c_str());
    rootElement->InsertEndChild(TypeCode);

    //WasteTime
    XMLElement* WasteTime = xmlDoc.NewElement("WasteTime");
    WasteTime->SetText(Tools::Time_t2String(invDetailInfo.WasteTime).c_str());
    rootElement->InsertEndChild(WasteTime);

    //keyFlagNo
    XMLElement* keyFlagNo = xmlDoc.NewElement("keyFlagNo");
    keyFlagNo->SetText((int) invDetailInfo.keyFlagNo);
    rootElement->InsertEndChild(keyFlagNo);

    XMLPrinter xmlPrinter;
    xmlDoc.Accept(&xmlPrinter);

    return (char *) xmlPrinter.CStr();
}

string Tools::Serialize(InvStockRepeat NoReadStock)
{
    XMLDocument xmlDoc;
    XMLDeclaration *xmlDec = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\"");
    xmlDoc.InsertEndChild(xmlDec);

    XMLElement* rootElement = xmlDoc.NewElement("InvStockRepeatInfo");
    xmlDoc.InsertEndChild(rootElement);

    //Kind
    XMLElement * Kind = xmlDoc.NewElement("Kind");
    Kind->SetText(NoReadStock.Kind);
    rootElement->InsertEndChild(Kind);

    //InvNoReadNum
    XMLElement * InvNoReadNum = xmlDoc.NewElement("InvNoReadNum");
    InvNoReadNum->SetText(NoReadStock.InvNoReadNum);
    rootElement->InsertEndChild(InvNoReadNum);

    //InvSucessReadNum
    XMLElement* InvSucessReadNum = xmlDoc.NewElement("InvSucessReadNum");
    InvSucessReadNum->SetText(NoReadStock.InvSucessReadNum);
    rootElement->InsertEndChild(InvSucessReadNum);


    //NoReadInvStocks
    XMLElement* NoReadInvStocks = xmlDoc.NewElement("NoReadInvStocks");
    rootElement->InsertEndChild(NoReadInvStocks);

    if (NoReadStock.ErrInfo.size() > 0)
    {
        for (vector<NoReadInvStock>::iterator begin = NoReadStock.ErrInfo.begin(), end = NoReadStock.ErrInfo.end(); begin != end; ++begin)
        {
            NoReadInvStock item = *begin;

            XMLElement* NoReadInvStockItems = xmlDoc.NewElement("NoReadInvStockItems");
            NoReadInvStocks->InsertEndChild(NoReadInvStockItems);

            //ErrNo
            XMLElement* ErrNo = xmlDoc.NewElement("ErrNo");
            ErrNo->SetText(item.ErrNo);
            NoReadInvStockItems->InsertEndChild(ErrNo);

            //InvCode
            XMLElement* InvCode = xmlDoc.NewElement("InvCode");
            InvCode->SetText(item.InvCode.c_str());
            NoReadInvStockItems->InsertEndChild(InvCode);

            //InvNo
            XMLElement* InvNo = xmlDoc.NewElement("InvNo");
            InvNo->SetText(item.InvNo);
            NoReadInvStockItems->InsertEndChild(InvNo);

            //Count
            XMLElement* Count = xmlDoc.NewElement("Count");
            Count->SetText(item.Count);
            NoReadInvStockItems->InsertEndChild(Count);
        }
    }


    XMLPrinter xmlPrinter;
    xmlDoc.Accept(&xmlPrinter);

    string xmlStr = xmlPrinter.CStr();
    string outData = Tools::UTF82GB18030(xmlPrinter.CStr());
    string Base64 = Tools::ToBase64String(outData);

    return Base64;
}

string Tools::Serialize(vector<InvoiceAmountType> AmountTypes)
{
    XMLDocument xmlDoc;
    XMLDeclaration *xmlDec = xmlDoc.NewDeclaration("xml version=\"1.0\" encoding=\"GBK\"");
    xmlDoc.InsertEndChild(xmlDec);

    XMLElement* rootElement = xmlDoc.NewElement("InvoiceAmountTypes");
    xmlDoc.InsertEndChild(rootElement);

    for (vector<InvoiceAmountType>::iterator begin = AmountTypes.begin(), end = AmountTypes.end(); begin != end; ++begin)
    {
        InvoiceAmountType AmountType = *begin;

        XMLElement* AmountTypeItem = xmlDoc.NewElement("AmountTypeItem");
        rootElement->InsertEndChild(AmountTypeItem);

        XMLElement * InvKind = xmlDoc.NewElement("InvType");
        InvKind->SetText(AmountType.InvType.c_str());
        AmountTypeItem->InsertEndChild(InvKind);

        XMLElement * KPMoney = xmlDoc.NewElement("KPMoney");
        KPMoney->SetText(AmountType.KPMoney.c_str());
        AmountTypeItem->InsertEndChild(KPMoney);

        XMLElement* TPMoney = xmlDoc.NewElement("TPMoney");
        TPMoney->SetText(AmountType.TPMoney.c_str());
        AmountTypeItem->InsertEndChild(TPMoney);

    }

    XMLPrinter xmlPrinter;
    xmlDoc.Accept(&xmlPrinter);
    string xmlStr = xmlPrinter.CStr();
    string outData = Tools::UTF82GB18030(xmlPrinter.CStr());
    string Base64 = Tools::ToBase64String(outData);
    return Base64;
}
///返回格式yyyy-MM-DD HH:MM:SS

string Tools::Time_t2String(time_t tmInfo)
{
    struct tm tm_kprq = Tools::time_t2tm(tmInfo);
    char tmp[50] = {0};
    sprintf(tmp, "%04d-%02d-%02d %02d:%02d:%02d", tm_kprq.tm_year, tm_kprq.tm_mon, tm_kprq.tm_mday, tm_kprq.tm_hour, tm_kprq.tm_min, tm_kprq.tm_sec);
    return tmp;
}

int Tools::GetByteCount(string data)
{
    return data.size();
}
//根据错误号获取错误描述

string Tools::GetMessageInfo(string msgID)
{
    string errcode = msgID;
    string errmsg = "";
    string errcode_upper = StringToUpperCase(msgID);
    string::size_type pos = errcode_upper.find("CA_");
    if (pos != errcode_upper.npos)
    {
        vector<string> vec = Split(msgID, "_");
        if (vec.size() < 2)
        {
            return errmsg;
        }
        errcode = vec[1];
        map<string, string>::iterator iter = ErrMsgMap_CA.find(errcode);
        if (iter != ErrMsgMap_CA.end())
        {
            errmsg = iter->second;
        }
    }
    else
    {
        string::size_type pos1 = errcode_upper.find("TCD");
        if (pos1 != errcode_upper.npos)
        {
            vector<string> vec = Split(msgID, "_");
            if (vec.size() < 2)
            {
                return errmsg;
            }
            errcode = vec[1];
        }
        map<string, string>::iterator iter = ErrMsgMap.find(errcode);
        if (iter != ErrMsgMap.end())
        {
            errmsg = iter->second;
        }
    }
    return errmsg;
}

bool Tools::IsDigitString(string data)
{
    if (data == "")
        return false;
    int len = data.size();
    for (int i = 0; i < len; ++i)
    {
        if (isdigit(data[i]) == false)
        {
            return false;
        }
    }
    return true;
}

/// <summary>
/// 校验通知单号,默认是专用发票
/// 通知单号为16位数字，其中第7-10位为年月
/// 最后一位为校验位
/// </summary>
/// <param name="tzdh">通知单号</param>
/// <returns></returns>

bool Tools::CheckTzdh(string tzdh)
{
    return CheckTzdh(tzdh, "s");
}



/// <summary>
/// 校验通知单号
/// 通知单号为16位数字，其中第7-10位为年月
/// 最后一位为校验位
/// </summary>
/// <param name="tzdh">通知单号</param>
/// <param name="fplx">发票类别： 专用发票 s， 货运发票 f </param>
/// <returns></returns>

bool Tools::CheckTzdh(string tzdh, string fplx)
{
    if (tzdh.size() == 0)
    {
        return false;
    }
    if (tzdh.size() > 16)
    {
        return false;
    }

    //校验是否为16位数字
    if (!IsDigitString(tzdh))
    {
        return false;
    }

    int year = Tools::Str2Int(tzdh.substr(6, 2));
    if (year < 7)
    {
        return false;
    }
    int month = Tools::Str2Int(tzdh.substr(8, 2));
    if (fplx != "s" && fplx != "f")
    {
        return false;
    }
    if (fplx == "s" && (month < 1 || month > 12))//增值税专票
    {
        return false;
    }
    if (fplx == "f" && (month < 13 || month > 24))//货运发票
    {
        return false;
    }
    int jyh = 0;
    int len = tzdh.size() - 1;
    for (int i = 0; i < len; i++)
    {
        string tmp(1, tzdh[i]);
        jyh += Tools::Str2Int(tmp);
    }
    string tmp2(1, tzdh[len]);
    if (jyh % 10 == Tools::Str2Int(tmp2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// <summary>
///  根据通知单号，生成红字发票备注信息
/// </summary>
/// <param name="tzdh"></param>
/// <param name="fplx"></param>
/// <returns></returns>

string Tools::GetRedInvNotes(string tzdh, string fplx)
{
    if (CheckTzdh(tzdh, fplx))
    {
        if (fplx == "s")
        {
            return PREFIX_TZDH_BZ + tzdh;
        }
        else if (fplx == "f")
        {
            return PREFIX_HYTZDH_BZ + tzdh;
        }
    }
    return "";
}

double Tools::SetDecimals(string data, short decimals)
{
    double data1 = Tools::Str2Double(data);
    return Tools::Round(data1, decimals);
}

string Tools::SetDecimalsString(string data, short decimals)
{
    return DoubleToString(SetDecimals(data, decimals));
}

/// <summary>
/// 检查两数值是否在误差范围内
/// </summary>
/// <param name="a">数值1</param>
/// <param name="b">数值2</param>
/// <param name="decimals">精度</param>
/// <param name="err">误差</param>
/// <returns></returns>

bool Tools::CheckErr(string a, string b, int decimals, string err)
{
    double astr = SetDecimals(a, decimals);
    double bstr = SetDecimals(b, decimals);

    astr -= bstr;
    double errd_str = fabs(astr);
    double errd = Str2Double(err);
    return errd_str <= errd;
}

Tools::Tools()
{
}

Tools::Tools(const Tools& orig)
{
}

Tools::~Tools()
{
}

vector<string> Tools::Split(const string& src, string separate_character)
{
    vector<string> strs;
    int separate_characterLen = separate_character.size(); //分割字符串长度，这样就可以支持多字 符串的分隔符
    int lastPosition = 0, index = -1;
    while (-1 != (index = src.find(separate_character, lastPosition)))
    {
        strs.push_back(src.substr(lastPosition, index - lastPosition));
        lastPosition = index + separate_characterLen;
    }
    string lastString = src.substr(lastPosition);
    if (!lastString.empty())
        strs.push_back(lastString);
    return strs;
}

string Tools::SubString(const string &src, int begin_pos, int end_pos)
{
    string sub = src.substr(begin_pos, end_pos - begin_pos);
    return sub;
}

string Tools::ReplaceAll(const string &src, string org_str, string rep_str)
{
    vector<string> delimVec = split(src, org_str);
    if (delimVec.size() <= 0)
    {
        cout << "can not find " << rep_str << endl;
        return src;
    }
    string target("");
    vector<string>::iterator it = delimVec.begin();
    for (; it != delimVec.end(); ++it)
    {
        target = target + (*it) + rep_str;
    }
    target = target + (*it);
    return target;
}

string Tools::FPLXToDBType(int fplx)
{
    switch (fplx)
    {
    case ZYFP:
        return "s";
    case PTFP:
        return "c";
    case HYFP:
        return "f";
    case JDCFP:
        return "j";
    case JSFP:
        return "q";
    case DZFP:
        return "p";
    default:
        return "s";
    }
}

int Tools::DBTypeToFPLX(string fplx)
{
    if (fplx == "s")
    {
        return ZYFP;
    }
    else if (fplx == "c")
    {
        return PTFP;
    }
    else if (fplx == "f")
    {
        return HYFP;
    }
    else if (fplx == "j")
    {
        return JDCFP;
    }
    else if (fplx == "p")
    {
        return DZFP;
    }
    else if (fplx == "q")
    {
        return JSFP;
    }
    else
    {
        return ZYFP;
    }
}

string Tools::ReplaceAll(const string &src, vector<string> &org_strVec, string rep_str)
{
    vector<string>::iterator pOrg = org_strVec.begin();
    string org_str(""), target = src;
    for (; pOrg != org_strVec.end(); ++pOrg)
    {
        org_str = *pOrg;
        vector<string> delimVec = split(target, org_str);
        target = "";
        if (delimVec.size() > 0)
        {
            vector<string>::iterator it = delimVec.begin();
            for (; it != delimVec.end() - 1; ++it)
            {
                target = target + (*it) + rep_str;
            }
            target = target + *(it);
        }
    }
    return target;
}

string Tools::ToUpper(string data)
{
    int i = 0;
    int len = data.size();
    while (i < len)
        data[i] = toupper(data[i++]);
    return data;
}

string Tools::ToLower(string data)
{
    int i = 0;
    int len = data.size();
    while (i < len)
        data[i] = tolower(data[i++]);
    return data;
}

struct tm Tools::time_t2tm(time_t dt)
{
    tm *pTmp = localtime(&dt);
    if (pTmp == NULL)
    {
        return *pTmp;
    }

    pTmp->tm_year += 1900;
    pTmp->tm_mon += 1;
    return *pTmp;
}

string Tools::TrimBlankSpace(string data)
{
    if (data.empty())
    {
        return data;
    }

    data.erase(0, data.find_first_not_of(" "));
    data.erase(data.find_last_not_of(" ") + 1);
    return data;
}

string Tools::TrimStart(string&data)
{
    if (data.empty())
    {
        return data;
    }

    data.erase(0, data.find_first_not_of(" "));
    return data;
}

string Tools::TrimEnd(string&data)
{
    if (data.empty())
    {
        return data;
    }

    data.erase(data.find_last_not_of(" ") + 1);
    return data;
}

//获取程序当前路径

string Tools::GetCurrentPath()
{
    const int path_max = 1024;
    char buf[path_max] = {0};
    int rslt = readlink("/proc/self/exe", buf, path_max);
    if (rslt < 0 || rslt >= path_max)
    {
        return "";
    }
    buf[rslt] = '\0';
    for (int i = rslt; i >= 0; i--)
    {
        if (buf[i] == '/')
        {
            buf[i + 1] = '\0';
            break;
        }
    }
    return string(buf);
}
//获取编译日期 yymmdd

string Tools::GetCompileDate()
{
    string compileDate(__DATE__);
    string compileTime(__TIME__);
    string Month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    vector<string> vec = split(compileDate, " ");
    if (vec.size() == 3)
    {
        string year = vec[2].substr(2, 2);
        string month = "00";
        for (int i = 0; i < 12; ++i)
        {
            if (vec[0] == Month[i])
            {
                month = FormatInt(i + 1, 2);
                break;
            }
        }
        string day = vec[1];
        vector<string> vecTime = split(compileTime, ":");
        string hour = vecTime[0];
        return "V4.0." + year + month + day + "(" + hour + ")";
    }
    else
    {
        return "4.0.0";
    }
}

string Tools::PadLeft(const string &srcStr, int totalLen, char padCh)
{
    if (totalLen <= srcStr.length())
        return srcStr;
    return string(totalLen - srcStr.length(), padCh) + srcStr;
}

string Tools::PadRight(const string &srcStr, int totalLen, char padCh)
{
    if (totalLen <= srcStr.length())
        return srcStr;
    return srcStr + string(totalLen - srcStr.length(), padCh);
}

/**
 * @brief 功能同sprintf
 * @param formatStr : sprintf的format串
 * @param value
 * @return
 */
string
Tools::SprintFormat(string formatStr, double value)
{
    char tmp[64] = {0};
    int tmpLen = sprintf(tmp, formatStr.c_str(), value);
    return string(tmp, tmpLen);
}

string Tools::SprintFormat(string formatStr, string value)
{
    return SprintFormat(formatStr, Str2Double(value));
}

/**
 * @brief 截取字符串指定的字节长度
 * @param s         原始字符串
 * @param len       需截取的字节长度
 * @param addTail   当len大于s的字节长度值时，尾部是否补加空格，直到字节长度达到len为止
 * @param firstLine 是否仅取原始字符串的第一行
 * @return
 */
string Tools::GetSubString(string s, int len, bool addTail, bool firstLine)
{
    string temp = s;
    if (firstLine && !s.empty())//如果仅取第一行
    {
        // 获取第一行，分隔符列表：{ "\r\n", "\n", "\r" }
        string::size_type pos = std::min(s.find_first_of("\r\n"), std::min(s.find_first_of("\n"), s.find_first_of("\r")));
        if (-1 == pos)
            temp = s;
        else
            temp = s.substr(0, pos);
    }

    if (temp.length() == 0)
    {
        if (!addTail)
            return "";
        else
            return string(len, ' ');
    }

    temp = UTF82GBK(temp);
    int bLen = Tools::GetByteCount(temp);
    if (bLen <= len)
    {
        if (addTail)
        {
            return Tools::PadRight(temp, len, ' ');
        }
        else
        {
            return temp;
        }
    }
    else
    {
        for (int i = temp.length(); i >= 0; i--)
        {
            temp = temp.substr(0, i);
            if (Tools::GetByteCount(temp) <= len)
            {
                return temp;
            }
        }
    }
    return "";

}

/**
 * @brief       封装md5.h的md5Encode
 * @param src   明文
 * @return      密文
 */
string
Tools::MD5Encode(string src)
{
    unsigned char cipher[16] = {0};
    md5Encode((unsigned char *) src.c_str(), src.length(), cipher);
    return string((const char *) cipher, 16);
}

int
Tools::GetFpbz(bool isZf, double dhjje)
{
    //正票
    if (dhjje > 0)
    {
        if (isZf)
            return 3;
        else
            return 0;
    }
    else if (dhjje < 0)//负票
    {
        if (isZf)
            return 4;
        else
            return 1;
    }
    else if (abs(dhjje) < 1E-8)
    {
        if (isZf)
            return 2;
        else
            return 0;
    }
    else
    {
        return 0;
    }
}

/*
 *lifanyue
 *20151022
 *截取2位小数
 */

string Tools::get2Precision(string value)
{
    string cacheValue = value;
    int pos = value.find_first_of('.');
    if (pos == 0)
        return value;
    if (value.substr(pos + 3, 1) <= "4")
    {
        value = value.substr(0, pos + 3);
    }
    else
    {
        value = value.substr(0, pos + 2) + Tools::Int2Str(Tools::Str2Int(value.substr(pos + 2, 1)) + 1);
    }
    return value;
}

