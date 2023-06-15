#include<QString>
#include<string>
#include<vector>
using namespace std;
enum STATE{
  MEET_CONTENT,
  MEET_BLANK,//空白字符
  MEET_QUOTE,//双引号
  MEET_ESCAPE_QUOTE,//双引号中的转义符
  MEET_ESCAPE_CONTENT
};
int str2arg(const QString&s,char*argv[]){
  int argc=0;
  if(!s.size())
    return argc;
  STATE st=MEET_BLANK;
  size_t cur=0;
  size_t cur_arg=0;
  for(;cur<s.size();++cur){
    switch(st){
    case MEET_BLANK:
      if(s[cur].unicode()==' '||s[cur].unicode()=='\t')
	break;
      cur_arg=0;      
      if(s[cur].unicode()=='"'){
	st=MEET_QUOTE;
	break;
      }
      if(s[cur].unicode()=='\\'){
	st=MEET_ESCAPE_CONTENT;
	break;
      }
      argv[argc][cur_arg++]=s[cur].unicode();
      if(cur==s.size()-1){
	argv[argc++][cur_arg++]=0;
	break;
      }
      st=MEET_CONTENT;
      break;
    case MEET_QUOTE:
      if(s[cur].unicode()=='"'){//结束双引号
	st=MEET_BLANK;
	argv[argc++][cur_arg++]=0;
	break;
      }
      if(s[cur].unicode()=='\\'){
	st=MEET_ESCAPE_QUOTE;
	break;
      }
      argv[argc][cur_arg++]=s[cur].unicode();
      break;
    case MEET_ESCAPE_QUOTE:{
      switch(s[cur].unicode()){
      case 'n':
	argv[argc][cur_arg++]='\n';
	break;
      case 't':
	argv[argc][cur_arg++]='\t';
	break;
      case 'v':
	argv[argc][cur_arg++]='\v';
	break;
      case 'a':
	argv[argc][cur_arg++]='\a';
	break;
      case 'b':
	argv[argc][cur_arg++]='\b';
	break;
      case 'f':
	argv[argc][cur_arg++]='\f';
	break;
      case 'r':
	argv[argc][cur_arg++]='\r';
	break;
      case '\\':
	argv[argc][cur_arg++]='\\';
	break;
      case '"':
	argv[argc][cur_arg++]='"';
	break;
      default:
	argc=1-cur;
	return argc;
      }
      st=MEET_QUOTE;
    }
      break;
    case MEET_ESCAPE_CONTENT:{
      switch(s[cur].unicode()){
      case 'n':
	argv[argc][cur_arg++]='\n';
	break;
      case 't':
	argv[argc][cur_arg++]='\t';
	break;
      case 'v':
	argv[argc][cur_arg++]='\v';
	break;
      case 'a':
	argv[argc][cur_arg++]='\a';
	break;
      case 'b':
	argv[argc][cur_arg++]='\b';
	break;
      case 'f':
	argv[argc][cur_arg++]='\f';
	break;
      case 'r':
	argv[argc][cur_arg++]='\r';
	break;
      case '\\':
	argv[argc][cur_arg++]='\\';
	break;
      case ' ':
	argv[argc][cur_arg++]=' ';	
	break;
      case '"':
	argv[argc][cur_arg++]='"';	
	break;	
      default:
	argc=1-cur;
	return argc;
      }
      st=MEET_CONTENT;
      if(cur==s.size()-1)
	++argc;
    }
      break;      
    case MEET_CONTENT:
      if(s[cur].unicode()==' '||s[cur].unicode()=='\t'){
	argv[argc++][cur_arg++]=0;
	st=MEET_BLANK;
	break;
      }
      if(s[cur].unicode()=='\\'){
	st=MEET_ESCAPE_CONTENT;
	break;
      }
      argv[argc][cur_arg++]=s[cur].unicode();
      if(cur==s.size()-1)
	argv[argc++][cur_arg++]=0;	
      break;
    default:
      argc=-cur;
      return argc;
    }
  }
  if(cur==s.size())
    --cur;
  if(st!=MEET_BLANK&&st!=MEET_CONTENT)
    argc=-cur;
  return argc;
}
