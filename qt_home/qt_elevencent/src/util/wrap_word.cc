
#include<string>
#include<QString>
#include<QFontMetrics>

using namespace std;

QString get_wrap_word(const QFontMetrics&font,const QString&text,int limitSize){
  limitSize-=font.horizontalAdvance("a");
  if(limitSize<font.horizontalAdvance("a"))
    limitSize=font.horizontalAdvance("a");
  QString ret="";
  size_t prev=0,cur=1;
  for(;cur<text.size();++cur){
    if(text[cur]=='\n'){
      ret+=text.mid(prev,cur-prev);
      prev=cur;
      continue;
    }
    if(font.horizontalAdvance(text.mid(prev,cur-prev))>=limitSize){
      ret+=text.mid(prev,cur-prev)+"\n";      
      prev=cur;
    }
  }
  ret+=text.mid(prev,cur-prev);
  return ret;
}
