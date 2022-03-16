#include <Arduino.h>
#include <Unicode2GB2312table.h>
// extern char myword[400];
int GetUtf8ByteNumForWord(uint8_t firstCh)
{
    if (firstCh > 0x80)
        return 3;
    else
        return 1;
}
uint16_t SearchCodeTable(uint16_t unicodeKey)
{
    int first = 0;
    int end = CODE_TABLE_SIZE - 1;
    int mid = 0;
    uint16 code_table_temp;
    while (first <= end)
    {
        mid = (first + end) / 2;
        code_table_temp = pgm_read_word_near(&code_table[mid].unicode);
        if (  code_table_temp == unicodeKey)
        {
            return pgm_read_word_near(&code_table[mid].gb);
        }
        else if (code_table_temp > unicodeKey)
        {
            end = mid - 1;
        }
        else
        {
            first = mid + 1;
        }
    }
    return 0xA1A1; //如果未查到此汉字或符号，就返回一个中文空格的gb2312码
}
// uint16_t SearchCodeTable(uint16_t unicodeKey)
// {
//     int first = 0;
//     int end = CODE_TABLE_SIZE - 1;
//     int mid = 0;
//     while (first <= end)
//     {
//         mid = (first + end) / 2;
//         if (code_table[mid].unicode == unicodeKey)
//         {
//             strcmp_P((char *)&unicodeKey, (char*)code_table[mid].unicode);
//             return code_table[mid].gb;
//         }
//         else if (code_table[mid].unicode > unicodeKey)
//         {
//             end = mid - 1;
//         }
//         else
//         {
//             first = mid + 1;
//         }
//     }
//     return 0;
// }
// void Utf8ToGb2312(const char *utf8, int len, uint16_t *gbArray)
// {
//     int k = 0;
//     int byteCount = 0;
//     int i = 0;
//     int j = 0;
//     char temp[10];
//     uint16_t unicodeKey = 0;
//     uint16_t gbKey = 0;
//     //循环解析
//     while (i < len)
//     {
//         switch (GetUtf8ByteNumForWord((uint8_t)utf8[i]))
//         {
//         case 0:
//             temp[j] = utf8[i];
//             byteCount = 1;
//             break;
//         case 2:
//             temp[j] = utf8[i];
//             temp[j + 1] = utf8[i + 1];
//             byteCount = 2;
//             break;
//         case 3:
//             //这里就开始进行UTF8->Unicode
//             temp[j + 1] = ((utf8[i] & 0x0F) << 4) | ((utf8[i + 1] >> 2) & 0x0F);
//             temp[j] = ((utf8[i + 1] & 0x03) << 6) + (utf8[i + 2] & 0x3F);
//             //取得Unicode的值
//             memcpy(&unicodeKey, (temp + j), 2);
//             //根据这个值查表取得对应的GB2312的值
//             gbKey = SearchCodeTable(unicodeKey);
//             //printf("gbKey=0x%X\n",gbKey);
//             gbArray[k++] = gbKey;
//             byteCount = 3;
//             break;
//         case 4:
//             byteCount = 4;
//             break;
//         case 5:
//             byteCount = 5;
//             break;
//         case 6:
//             byteCount = 6;
//             break;
//         default:
//             printf("the len is more than 6\n");
//             break;
//         }
//         i += byteCount;
//         if (byteCount == 1)
//         {
//             j++;
//         }
//         else
//         {
//             j += 2;
//         }
//     }
//     //return gbKey;
// }
void GetGb2312String(char *utf_8, char *myword){
    int i = 0;
    int j = 0;
    int len = strlen(utf_8);
    char temp[10]; 
    uint16_t unicodeKey = 0;
    uint16_t gb2312Key = 0;
    while (i < len)
    {
        int byte_num = GetUtf8ByteNumForWord((uint8_t)utf_8[i]);
        if (byte_num == 1)
        {
            myword[j] = utf_8[i];
            i++; j++;
        }
        else if (byte_num == 3)
        {
            //utf-8转为unicode
            temp[1] = ((utf_8[i] & 0x0F) << 4) | ((utf_8[i + 1] >> 2) & 0x0F);
            temp[0] = ((utf_8[i + 1] & 0x03) << 6) + (utf_8[i + 2] & 0x3F);
            memcpy(&unicodeKey, temp, 2);
            gb2312Key = SearchCodeTable(unicodeKey);
            // Serial.printf("unicodeKey:%X ", unicodeKey);
            // Serial.printf("gb2312Key:%X ", gb2312Key);
            char* ptr_gb2312Key = (char *)&gb2312Key;
            myword[j+1] = *ptr_gb2312Key;
            // Serial.print((uint8_t)myword[j]);
            ptr_gb2312Key++;
            myword[j] = *ptr_gb2312Key;
            // Serial.print((uint8_t)myword[j+1]);
            // Serial.printf("myword GB2312: %#X", *ptr_myword);
            i += 3; j += 2;
        }
        else
        {
            i++;
        } 
    }
    //字符串末尾加\0
    if(utf_8[i] > 0x80)
        myword[j+1]=0;
    else
        myword[j]=0;

}









