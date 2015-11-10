/* 
 * File:   Tools.h
 * Author: jsy
 *
 * Created on 2015年9月19日, 下午4:14
 */

#ifndef TOOLS_H
#define	TOOLS_H
#include <stddef.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <list>
#include <tinyxml2/tinyxml2.h>
#include "TaxCardIESDefine.h"
#include "Fpxx.h"
#include "md5.h"

using namespace tinyxml2;

using std::string;
using std::vector;

class Tools {
private:
    static string PREFIX_TZDH_BZ;
    static string PREFIX_HYTZDH_BZ;
    static string PREFIX_FPDM_BZ;
    static string PREFIX_FPHM_BZ;
    static string PREFIX_DKQYSH;
    static string PREFIX_DKQYMC;
    

public:
    static int MAXPERIOD;
    Tools();
    Tools(const Tools& orig);
    //字符编码转换（UTF-8/GBK/ISO8859-1）
    static int ConvertCharSet(const char *dest,
            const char *src,
            char *input,
            size_t ilen,
            char *output,
            size_t olen);
    //int to string
    static string Int2Str(int value);
    //long to string
    static string Long2Str(long value);
    //float to string
    static string Float2Str(double value, int precision = 2);
    //string to int
    static int Str2Int(string value);
    //string to long
    static long Str2Long(string value);
    //string to float
    static double Str2Float(string value);
    //四舍五入（将double保留precision）
    static string Bool2Int(bool value);


    static double GetRound(double value, int precision = 2);
    //四舍五入（将string转double，保留precision）
    static double GetRound(string value, int precision = 2);
    //字符校验 校验二维码数据里面有没有 GBK字符集范围内的不可见字符
    static bool CheckDataStr_GBK(string dataBuf);
    //截取子串
    //len截取子串的长度
    static string SubChStr(string str, int len, bool inputGBK = false, bool outputGBK = false);
    //是否全为数字
    static bool IsDigit(string str);
    //trim string
    static string Trim(const string str);
    //格式化整数 FormatInt(1, 3) = "001"
    static string FormatInt(int value, int len);
    //给字符串以空格增加到指定长度
    //aNum 字符串指定的长度
    //在末尾添加
    static string Addblank(string str, int size);
    //将str中的字符，逐字写入vs
    static void StringToWords(const string str, vector<string> &vs);
    //中英文混排字符个数统计和编码
    static int ChrHZCount(string str, string &last);
    //字符串替换
    static string Replace(string src, string searchStr, string replaceStr);
    //将某一字符替换为传入的字符串	index从0开始
    static string Replace(string src, int index, string replaceStr);
    //字符串中如果有回车换成空格
    static string DealReturnStr(string aStr, bool isRepaired = false);
    //对于SQL语句,对'号进行处理  postgresql    //
    static string DealSqlStr(string str);
    //GBK转UTF8
    static string GBK2UTF8(const string str, int len = 0);
    //UTF8转GBK
    static string UTF82GBK(const string str, int len = 0);
    //UTF8转GB18030
    static string UTF82GB18030(const string str, int len = 0);
    //编码转换
    static string EncodingConvert(const string str, string fromEncoding, string toEncoding, int len);
    //字符串以16进制输出
    static string ToHexString(const string &str);

    static unsigned char* Hex2Bytes(const string& hexStr);
    
    static void Hex2Bytes2(const string& hexStr,unsigned char* byteArray,int &leng);

    static string BytesToHex(char* byteArray, unsigned long len);

    static string Bytes2Hex(unsigned char* byteArray);

    //String 转换为 Double
    static double Str2Double(const string data);

    //String 转换为 bool 类型
    static bool Str2Bool(const string data);

    //小写转换为大写
    static string StringToUpperCase(string data);
    //大写转换为小写
    static string StringToLowerCase(string data);

    static time_t StringToDateTime(string data);
    static string NowTimeToString(time_t iTimeStamp);
    static string NowTimeToString1(time_t iTimeStamp);
    static time_t stringTOtime_t(const char * szTime);
    static string FPHMTo8Wei(string strValue);
    static string FPHMTo8Wei(int intValue);
    static string formatLengthStr(int intValue, int length);
    static string formatLengthStr(string strValue, int length);
    static void GetYYSBZ(Fpxx fpxx);
    static string FPLXToDBType(FPLX fplx);
    static string FPLXToDBType(string fplx);
    static string DBTypeToFPLX(char dbType);
    static int FPLXToCardType(string fplx);
    static int GetReturnErrCode(string errcode);
    static string GetStatusCode();
    static void ReplaceAll(string&s1, const string&s2, const string&s3);
    static void ReplaceAll_SQL(string&s1, const string&s2, const string&s3);
    static string TrimBlankSpace(string data);
    static string TrimEnd(string&data);
    static string TrimStart(string&data);


    static const int OUT_BUF_LEN = 1024 * 20;
    static const int IN_BUF_LEN = 1024 * 10;

    static string FormatDateTime(time_t t);

    //add by jsy
    static vector<string> split(string str, string separator);
    static string GetString(string data);
    static string GetString(string data, int index, int len);
    static string GetInfo(string data, int fpzl, string seperator = "");
    static string GetBytes(string data);
    static string Add(string data1, string data2);
    static char * Copy(char * src, int index, char * buf, int len);
    static void GetDKQYFromInvNotes(string data, string & outsh, string & outmc);
    static string GetSubDecimal(string data, int len, bool flag);
    static string MultiplyStr(string data1, string data2, int len);
    static string Multiply(string data1, string data2, int len);
    static string DivideStr(string data1, string data2, int len);
    static string FromBase64String(string str);
    static string ToBase64String(string data);
    static double Round(double dVal, short decimals);
    static string DoubleToString(double data);
    static double Round(string dVal, short decimals);
    static string Serialize(vector<ClearCardInfo> clearList);
    static string Serialize(ClearCardInfo clearInfo);
    static string Serialize(CSDateInfo csInfo);
    static string Serialize(InvDetail csInfo);
    static string Serialize(InvVolumeApp ivvApp);
    static string Serialize(InvStockRepeat NoReadStock);
    static string Serialize(vector<InvoiceAmountType> AmountTypes);
    static string Time_t2String(time_t tmInfo);
    static bool CheckTzdh(string data);
    static bool CheckTzdh(string tzdh, string fplx);
    static string GetRedInvNotes(string tzdh, string fplx = "s");
    static int GetByteCount(string data);
    static bool IsDigitString(string data);
    static double SetDecimals(string data, short decimals);
    static string SetDecimalsString(string data, short decimals);
    static bool CheckErr(string a, string b, int decimals, string err);
    static string FPLXToDBType(int fplx);
    static int DBTypeToFPLX(string fplx);
    static string ToUpper(string data);
    static string ToLower(string data);
    static struct tm time_t2tm(time_t dt);
    static string AES_Erypt(string data, byte key[16], byte vec[16]);
    static string AES_Decrypt(string data, byte key[16], byte vec[16], string flag  ="");
    
    /// <summary>
    /// 获取data前面的数字
    /// </summary>
    /// <param name="data"></param>
    /// <returns></returns>
    static string GetPreDigital(string data);

    //根据错误号获取错误描述
    static string GetMessageInfo(string msgID);
    virtual ~Tools();

    static vector<string> Split(const string &src, string separate_character);
    static string SubString(const string &src, int begin_pos, int end_pos);
    static string ReplaceAll(const string &src, string org_str, string rep_str);
    static string ReplaceAll(const string &src, vector<string> &org_strVec, string rep_str);
    //获取程序当前路径
    static string GetCurrentPath();
    //获取编译日期 yymmdd
    static string GetCompileDate();
    static string PadLeft(const string &srcStr, int totalLen, char padCh = '0');
    static string PadRight(const string &srcStr, int totalLen, char padCh = '0');
    static string SprintFormat(string formatStr, double value);
    static string SprintFormat(string formatStr, string value);
    static string UTF8ToGB(const char* str, int &retcode);
    static string GetSubString(string s, int len, bool addTail, bool firstLine = true);
    static vector<string> MutiSplit(const string &src, vector<string> vec_separator, bool removeEmptyEntries = false);
    static string MD5Encode(string src);
    static int GetFpbz(bool isZf, double dhjje);
    static string get2Precision(string value);
    static string formatDoubleString (double doubleValue,int precision);
    
    static string formatDoubleString (string doubleValue,int precision);
    
private:

};

#endif	/* TOOLS_H */
