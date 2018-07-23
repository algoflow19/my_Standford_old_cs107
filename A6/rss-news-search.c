#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "url.h"
#include "bool.h"
#include "urlconnection.h"
#include "streamtokenizer.h"
#include "html-utils.h"
#include"hashset.h"
#include"stirngHashSet.h"
#include "queryHashSet.h"
#include<ctype.h>
#include<pthread.h>

#define MAX_FEED_INPROCESS_NUM 10

typedef struct joinChecker {
  int threadNum;
  pthread_mutex_t lock;
  pthread_t singleList[100];
} joinChecker;

static void Welcome(const char *welcomeTextFileName);
static void BuildIndices(const char *feedsFileName, hashset *titleSet, hashset *urlSet, hashset *querySet, hashset *stopSet);
typedef struct processFeedAuxData{
  const char *remoteDocumentName;
  hashset *titleSet;
  hashset *urlSet;
  hashset *querySet;
  hashset *stopSet;
  pthread_mutex_t *queryMut;
  pthread_mutex_t *titleMut;
  pthread_mutex_t *urlMut;
  pthread_mutex_t *stopWordMut;
  pthread_mutex_t *ovalLayerWatcher;
} processFeedAuxData;

static void ProcessFeed(const char *remoteDocumentName, hashset *titleSet, hashset *urlSet, hashset *querySet, hashset *stopSet, pthread_mutex_t *queryMut, pthread_mutex_t *titleMut, pthread_mutex_t *urlMut, pthread_mutex_t *stopWordMut);
static void threadProcessFeed(processFeedAuxData* data){
  char remoteDocumentName[1024];
  memcpy(remoteDocumentName,data->remoteDocumentName,strlen(data->remoteDocumentName)+1);
  pthread_mutex_unlock(data->ovalLayerWatcher);
  ProcessFeed(remoteDocumentName,
                     data->titleSet,
                     data->urlSet,
                     data->querySet,
                     data->stopSet,
                     data->queryMut,
                     data->titleMut,
                     data->urlMut,
                     data->stopWordMut);
}

static void PullAllNewsItems(urlconnection *urlconn, hashset *titleSet, hashset *urlSet, hashset *querySet, hashset *stopSet, pthread_mutex_t *queryMut, pthread_mutex_t *titleMut, pthread_mutex_t *urlMut, pthread_mutex_t *stopWordMut);
static bool GetNextItemTag(streamtokenizer *st);
static void ProcessSingleNewsItem(streamtokenizer *st, hashset *titleSet, hashset *urlSet, hashset *querySet, hashset *stopSet, pthread_mutex_t *queryMut, pthread_mutex_t *titleMut, pthread_mutex_t *urlMut, pthread_mutex_t *stopWordMut, joinChecker *checker);
static void ExtractElement(streamtokenizer *st, const char *htmlTag, char dataBuffer[], int bufferLength);
typedef struct singleArticleAuxData{
  const char *articleTitle;
  const char *articleURL;
  hashset *querySet;
  hashset *stopSet;
  pthread_mutex_t *queryMut;
  pthread_mutex_t *stopWordMut;
} singleArticleAuxData;

static void ParseArticle(const char *articleTitle, const char *articleURL,hashset* querySet,hashset* stopSet,
                         pthread_mutex_t *quereMut,pthread_mutex_t *stopWordMut);

static void threadParseArticle(singleArticleAuxData* data){
  printf ("\nThe articleTitle pass to the thread is is:%s and the number is \n",data->articleTitle);
  ParseArticle (data->articleTitle,
                data->articleURL,
                data->querySet,
                data->stopSet,
                data->queryMut,
                data->stopWordMut
                );
  free(data);
}
static void ScanArticle(streamtokenizer *st, const char *articleTitle, const char *articleURL, hashset *querySet, hashset *stopSet, pthread_mutex_t *quereMut, pthread_mutex_t *stopWordMut);
static void QueryIndices(hashset *querySet, hashset *stopSet);
static void ProcessResponse(const char *word,hashset *querySet, hashset *stopSet);
static bool WordIsWellFormed(const char *word);

static const char *const kNewLineDelimiters = "\r\n";
static void changeToLowCase( char *target)
{
  int len=strlen(target);
  for(int i=0;i<len;i++)
    target[i]=tolower(target[i]);
}
/*************************
 * wordParseFun
*/

static int wordUnitCompareFun(void* elemaddr1,void* elemaddr2){
    assert(elemaddr1!=NULL&&elemaddr2!=NULL);
    return strcmp(((struct wordUnit*)elemaddr1)->word,((struct wordUnit*)elemaddr2)->word);
}

static void wordUnitFreeFun(void* element){

    struct wordUnit* unit=element;
    VectorDispose(unit->posList);
    free(unit->word);
}

static int wordUnitHash(char **s, int numBuckets)
{
  int i;
  unsigned long hashcode = 0;

  for (i = 0; i < strlen(*s); i++)
      hashcode = hashcode * kHashMultiplier + tolower((*s)[i]);
  return hashcode % numBuckets;

}


static int posTitleCompareFun(void *elemaddr1,void *elemaddr2){
  struct pos *p1=elemaddr1;
  struct pos *p2=elemaddr2;
  return strcmp(p1->title,p2->title);
}

static int posConutCompareFun(const void *elemaddr1,const void *elemaddr2){
  struct pos *p1=elemaddr1;
  struct pos *p2=elemaddr2;
  return p1->count-p2->count;
}

void wordUnitSortPosByCount(void *elemAddr, void *auxData){
  struct wordUnit *unit=elemAddr;
  VectorSort (unit->posList,posConutCompareFun);
}

/**
 * Function: main
 * --------------
 * Serves as the entry point of the full application.
 * You'll want to update main to declare several hashsets--
 * one for stop words, another for previously seen urls, etc--
 * and pass them (by address) to BuildIndices and QueryIndices.
 * In fact, you'll need to extend many of the prototypes of the
 * supplied helpers functions to take one or more hashset *s.
 *
 * Think very carefully about how you're going to keep track of
 * all of the stop words, how you're going to keep track of
 * all the previously seen articles, and how you're going to
 * map words to the collection of news articles where that
 * word appears.
 */

static const char *const kWelcomeTextFile = "/home/kk/Music/assn-4-rss-news-search/welcome.txt";
static const char *const kDefaultFeedsFile = "/home/kk/Music/assn-4-rss-news-search/rss-feeds.txt";
static const char *const kDefaultStopWordsFile = "/home/kk/Music/assn-4-rss-news-search-data/stop-words.txt";

int main(int argc, char **argv)
{

  Welcome(kWelcomeTextFile);

  hashset *titleSet=malloc(sizeof(hashset));
  hashset *urlSet=malloc(sizeof(hashset));
  hashset *stopSet=malloc(sizeof(hashset));
  hashset *querySet=malloc(sizeof(hashset));
  HashSetNew(titleSet,sizeof(char*),1009,StringHash,stringCompareFun,stringFreeFun);
  HashSetNew(urlSet,sizeof(char*),1009,StringHash,stringCompareFun,stringFreeFun);
  HashSetNew(querySet,sizeof(struct wordUnit),10007,wordUnitHash,wordUnitCompareFun,wordUnitFreeFun);
  HashSetNew(stopSet,sizeof(char*),1009,StringHash,stringCompareFun,stringFreeFun);

  FILE* stopFile=fopen (kDefaultStopWordsFile,"r+");
  if(stopFile==NULL){
      printf ("open Stopwords file fail... stop");
      exit(-2);
    }
  streamtokenizer st;
  STNew (&st,stopFile,kNewLineDelimiters,true);
  char line[128];
  while (STNextToken (&st,line,256)) {
      char **buffer=malloc(sizeof(char*));
      *buffer=malloc(strlen(line)+1);
      memcpy(*buffer,line,strlen(line)+1);
      HashSetEnter (stopSet,buffer);
      free(buffer);
    }

  BuildIndices((argc == 1) ? kDefaultFeedsFile : argv[1],titleSet,urlSet,querySet,stopSet);
  QueryIndices(querySet,stopSet);
  return 0;
  STDispose(&st);
  HashSetDispose(titleSet);
  HashSetDispose(urlSet);
  HashSetDispose(querySet);
  HashSetDispose(stopSet);

}

/**
 * Function: Welcome
 * -----------------
 * Displays the contents of the specified file, which
 * holds the introductory remarks to be printed every time
 * the application launches.  This type of overhead may
 * seem silly, but by placing the text in an external file,
 * we can change the welcome text without forcing a recompilation and
 * build of the application.  It's as if welcomeTextFileName
 * is a configuration file that travels with the application.
 */


static void Welcome(const char *welcomeTextFileName)
{
  FILE *infile;
  streamtokenizer st;
  char buffer[1024];

  infile = fopen(welcomeTextFileName, "r");
  assert(infile != NULL);

  STNew(&st, infile, kNewLineDelimiters, true);
  while (STNextToken(&st, buffer, sizeof(buffer))) {
    printf("%s\n", buffer);
  }

  printf("\n");
  STDispose(&st); // remember that STDispose doesn't close the file, since STNew doesn't open one..
  fclose(infile);
}




void joinNThread(pthread_t t[],int elemNum){
  for(int i=0;i<elemNum;i++)
    pthread_join (t[i],NULL);
}

/**
 * Function: BuildIndices
 * ----------------------
 * As far as the user is concerned, BuildIndices needs to read each and every
 * one of the feeds listed in the specied feedsFileName, and for each feed parse
 * content of all referenced articles and store the content in the hashset of indices.
 * Each line of the specified feeds file looks like this:
 *
 *   <feed name>: <URL of remore xml document>
 *
 * Each iteration of the supplied while loop parses and discards the feed name (it's
 * in the file for humans to read, but our aggregator doesn't care what the name is)
 * and then extracts the URL.  It then relies on ProcessFeed to pull the remote
 * document and index its content.
 */

static void BuildIndices(const char *feedsFileName,
                         hashset* titleSet,
                         hashset* urlSet,
                         hashset* querySet,
                         hashset* stopSet
                         )
{
  FILE *infile;
  streamtokenizer st;
  char remoteFileName[1024];

  infile = fopen(feedsFileName, "r");
  assert(infile != NULL);
  STNew(&st, infile, kNewLineDelimiters, true);

  pthread_mutex_t queryMut = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t titleMut = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t urlMut=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t stopWordMut=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t wather=PTHREAD_MUTEX_INITIALIZER;
  pthread_t FeedthreadList[MAX_FEED_INPROCESS_NUM];
  int i=0;
  processFeedAuxData data={ remoteFileName,titleSet,urlSet,querySet,stopSet,
                            &queryMut,
                            &titleMut,
                            &urlMut,
                            &stopWordMut,
                            &wather
                          };
  while (STSkipUntil(&st, ":") != EOF) { // ignore everything up to the first selicolon of the line
      if(i==MAX_FEED_INPROCESS_NUM){
          joinNThread (FeedthreadList,MAX_FEED_INPROCESS_NUM);
          i=0;
        }
      STSkipOver(&st, ": ");		 // now ignore the semicolon and any whitespace directly after it
      pthread_mutex_lock( data.ovalLayerWatcher);
      data.remoteDocumentName=strdup(remoteFileName);
      STNextToken(&st, remoteFileName, sizeof(remoteFileName));
      pthread_create (FeedthreadList+i,NULL,threadProcessFeed,&data);
      pthread_mutex_unlock( data.ovalLayerWatcher);
      i++;
    }
  joinNThread (FeedthreadList,i);
  STDispose(&st);
  fclose(infile);
  printf("\n");

  HashSetMap (querySet,wordUnitSortPosByCount,NULL);
  int hashSetSize=HashSetCount (querySet);
  assert(hashSetSize>0&&hashSetSize<=10007);
  printf("We have %d words to be check!\n",hashSetSize);

}




/**
 * Function: ProcessFeed
 * ---------------------
 * ProcessFeed locates the specified RSS document, and if a (possibly redirected) connection to that remote
 * document can be established, then PullAllNewsItems is tapped to actually read the feed.  Check out the
 * documentation of the PullAllNewsItems function for more information, and inspect the documentation
 * for ParseArticle for information about what the different response codes mean.
 */

static void ProcessFeed(const char *remoteDocumentName,hashset* titleSet,hashset* urlSet,hashset* querySet,hashset* stopSet,
                        pthread_mutex_t* queryMut,
                        pthread_mutex_t* titleMut,
                        pthread_mutex_t* urlMut,
                        pthread_mutex_t* stopWordMut
                        )
{
  url u;
  urlconnection urlconn;
  char commond[256];
  sprintf(commond," curl -x 127.0.0.1:9666 %s -o %s ",remoteDocumentName,"a.xml");
  printf (" %s \n",commond);
  system (commond);
  urlconn.dataStream=fopen ("a.xml","r+");
  urlconn.responseCode=200;
  //URLNewAbsolute(&u, remoteDocumentName);
  //URLConnectionNew(&urlconn, &u);

  switch (urlconn.responseCode) {
      case 0: printf("Unable to connect to \"%s\".  Ignoring...", u.serverName);
              break;
      case 200: PullAllNewsItems(&urlconn,titleSet,urlSet,querySet,stopSet,queryMut,
                                 titleMut,urlMut,stopWordMut);
                break;
      case 301:
      case 302: ProcessFeed(urlconn.newUrl,titleSet,urlSet,querySet,stopSet,queryMut,
                            titleMut,
                            urlMut,
                            stopWordMut);
                break;
      default: printf("Connection to \"%s\" was established, but unable to retrieve \"%s\". [response code: %d, response message:\"%s\"]\n",
              u.serverName, u.fileName, urlconn.responseCode, urlconn.responseMessage);
           break;
  };

  URLConnectionDispose(&urlconn);
  URLDispose(&u);
}

/**
 * Function: PullAllNewsItems
 * --------------------------
 * Steps though the data of what is assumed to be an RSS feed identifying the names and
 * URLs of online news articles.  Check out "datafiles/sample-rss-feed.txt" for an idea of what an
 * RSS feed from the www.nytimes.com (or anything other server that syndicates is stories).
 *
 * PullAllNewsItems views a typical RSS feed as a sequence of "items", where each item is detailed
 * using a generalization of HTML called XML.  A typical XML fragment for a single news item will certainly
 * adhere to the format of the following example:
 *
 * <item>
 *   <title>At Installation Mass, New Pope Strikes a Tone of Openness</title>
 *   <link>http://www.nytimes.com/2005/04/24/international/worldspecial2/24cnd-pope.html</link>
 *   <description>The Mass, which drew 350,000 spectators, marked an important moment in the transformation of Benedict XVI.</description>
 *   <author>By IAN FISHER and LAURIE GOODSTEIN</author>
 *   <pubDate>Sun, 24 Apr 2005 00:00:00 EDT</pubDate>
 *   <guid isPermaLink="false">http://www.nytimes.com/2005/04/24/international/worldspecial2/24cnd-pope.html</guid>
 * </item>
 *
 * PullAllNewsItems reads and discards all characters up through the opening <item> tag (discarding the <item> tag
 * as well, because once it's read and indentified, it's been pulled,) and then hands the state of the stream to
 * ProcessSingleNewsItem, which handles the job of pulling and analyzing everything up through and including the </item>
 * tag. PullAllNewsItems processes the entire RSS feed and repeatedly advancing to the next <item> tag and then allowing
 * ProcessSingleNewsItem do process everything up until </item>.
 */


static const char *const kTextDelimiters = " \t\n\r\b!@$%^*()_+={[}]|\\'\":;/?.>,<~`";
static void PullAllNewsItems(urlconnection *urlconn,hashset* titleSet,hashset* urlSet,hashset* querySet,hashset* stopSet,
                             pthread_mutex_t* queryMut,
                             pthread_mutex_t* titleMut,
                             pthread_mutex_t* urlMut,
                             pthread_mutex_t* stopWordMut)
{

  joinChecker checker;
  checker.threadNum=0;
  pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
  checker.lock=lock;
  streamtokenizer st;
  STNew(&st, urlconn->dataStream, kTextDelimiters, false);
  int i=200;
  while (GetNextItemTag(&st)) { // if true is returned, then assume that <item ...> has just been read and pulled from the data stream
      i--;
    ProcessSingleNewsItem(&st,titleSet,urlSet,querySet,stopSet,queryMut,
                          titleMut,urlMut,stopWordMut,&checker);
    if(i==0) break;
  }
  for(int i=0;i<checker.threadNum;i++)
    pthread_join (checker.singleList[i],NULL);

  STDispose(&st);
}

/**
 * Function: GetNextItemTag
 * ------------------------
 * Works more or less like GetNextTag below, but this time
 * we're searching for an <item> tag, since that marks the
 * beginning of a block of HTML that's relevant to us.
 *
 * Note that each tag is compared to "<item" and not "<item>".
 * That's because the item tag, though unlikely, could include
 * attributes and perhaps look like any one of these:
 *
 *   <item>
 *   <item rdf:about="Latin America reacts to the Vatican">
 *   <item requiresPassword=true>
 *
 * We're just trying to be as general as possible without
 * going overboard.  (Note that we use strncasecmp so that
 * string comparisons are case-insensitive.  That's the case
 * throughout the entire code base.)
 */

static const char *const kItemTagPrefix = "<item";
static bool GetNextItemTag(streamtokenizer *st)
{
  char htmlTag[1024];
  htmlTag[0]='\0';
  while (GetNextTag(st, htmlTag, sizeof(htmlTag))) {
    if (strncasecmp(htmlTag, kItemTagPrefix, strlen(kItemTagPrefix)) == 0) {
      return true;
    }
  }
  return false;
}

/**
 * Function: ProcessSingleNewsItem
 * -------------------------------
 * Code which parses the contents of a single <item> node within an RSS/XML feed.
 * At the moment this function is called, we're to assume that the <item> tag was just
 * read and that the streamtokenizer is currently pointing to everything else, as with:
 *
 *      <title>Carrie Underwood takes American Idol Crown</title>
 *      <description>Oklahoma farm girl beats out Alabama rocker Bo Bice and 100,000 other contestants to win competition.</description>
 *      <link>http://www.nytimes.com/frontpagenews/2841028302.html</link>
 *   </item>
 *
 * ProcessSingleNewsItem parses everything up through and including the </item>, storing the title, link, and article
 * description in local buffers long enough so that the online new article identified by the link can itself be parsed
 * and indexed.  We don't rely on <title>, <link>, and <description> coming in any particular order.  We do asssume that
 * the link field exists (although we can certainly proceed if the title and article descrption are missing.)  There
 * are often other tags inside an item, but we ignore them.
 */

static const char *const kItemEndTag = "</item>";
static const char *const kTitleTagPrefix = "<title";
static const char *const kDescriptionTagPrefix = "<description";
static const char *const kLinkTagPrefix = "<link";
static void ProcessSingleNewsItem(streamtokenizer *st,hashset* titleSet,hashset* urlSet,hashset* querySet,hashset* stopSet,
                                  pthread_mutex_t* queryMut,
                                  pthread_mutex_t* titleMut,
                                  pthread_mutex_t* urlMut,
                                  pthread_mutex_t* stopWordMut,
                                  joinChecker* checker)
{
  char htmlTag[1024];
  char articleTitle[1024];
  char articleDescription[2048];
  char articleURL[1024];
  articleTitle[0] = articleDescription[0] = articleURL[0] = '\0';


  while (GetNextTag(st, htmlTag, sizeof(htmlTag)) && (strcasecmp(htmlTag, kItemEndTag) != 0)) {
    if (strncasecmp(htmlTag, kTitleTagPrefix, strlen(kTitleTagPrefix)) == 0) ExtractElement(st, htmlTag, articleTitle, sizeof(articleTitle));
    if (strncasecmp(htmlTag, kDescriptionTagPrefix, strlen(kDescriptionTagPrefix)) == 0) ExtractElement(st, htmlTag, articleDescription, sizeof(articleDescription));
    if (strncasecmp(htmlTag, kLinkTagPrefix, strlen(kLinkTagPrefix)) == 0) ExtractElement(st, htmlTag, articleURL, sizeof(articleURL));
  }

  printf("%s \n",articleTitle);
  printf("%s \n",articleDescription);
  printf("%s \n",articleURL);
  if (strlen(articleURL) == 0) return;     // punt, since it's not going to take us anywhere

  char* pass=articleURL; /* In fact, the addr of store array itself is not reachable. */
  pthread_mutex_lock(urlMut);
  if(HashSetLookup(urlSet,&pass)!=NULL){
      pthread_mutex_unlock(urlMut);
      return;
    }

  char* urlCopy=malloc(strlen(articleURL)+1);
  memcpy(urlCopy,articleURL,strlen(articleURL)+1);
  changeToLowCase (urlCopy);
  HashSetEnter (urlSet,&urlCopy);  
  pthread_mutex_unlock(urlMut);

  /*
   * Simply ignore article with the same title, because we only fetch articles from the same server. (Poor Network).
   */

  pass=articleTitle;
  pthread_mutex_lock(titleMut);
  if(HashSetLookup(titleSet,&pass)!=NULL){
      pthread_mutex_unlock(titleMut);
      return;
    }
  char* titleCopy=malloc(strlen(articleTitle)+1);
  malloc(strlen(articleTitle)+1);
  memcpy(titleCopy,articleTitle,strlen(articleTitle)+1);
  changeToLowCase (titleCopy);
  HashSetEnter(titleSet,&titleCopy);
  pthread_mutex_unlock(titleMut);



  singleArticleAuxData *aux=malloc(sizeof(singleArticleAuxData));
  singleArticleAuxData tmp={titleCopy,urlCopy,querySet,stopSet,queryMut,stopWordMut};
  *aux=tmp;
  printf ("The titleCopy is:%s and the number is %d \n",titleCopy,checker->threadNum);
  pthread_create (&checker->singleList[checker->threadNum],NULL,threadParseArticle,aux);
  checker->threadNum++;

}

/**
 * Function: ExtractElement
 * ------------------------
 * Potentially pulls text from the stream up through and including the matching end tag.  It assumes that
 * the most recently extracted HTML tag resides in the buffer addressed by htmlTag.  The implementation
 * populates the specified data buffer with all of the text up to but not including the opening '<' of the
 * closing tag, and then skips over all of the closing tag as irrelevant.  Assuming for illustration purposes
 * that htmlTag addresses a buffer containing "<description" followed by other text, these three scanarios are
 * handled:
 *
 *    Normal Situation:     <description>http://some.server.com/someRelativePath.html</description>
 *    Uncommon Situation:   <description></description>
 *    Uncommon Situation:   <description/>
 *
 * In each of the second and third scenarios, the document has omitted the data.  This is not uncommon
 * for the description data to be missing, so we need to cover all three scenarious (I've actually seen all three.)
 * It would be quite unusual for the title and/or link fields to be empty, but this handles those possibilities too.
 */

static void ExtractElement(streamtokenizer *st, const char *htmlTag, char dataBuffer[], int bufferLength)
{
  assert(htmlTag[strlen(htmlTag) - 1] == '>');
  if (htmlTag[strlen(htmlTag) - 2] == '/') return;    // e.g. <description/> would state that a description is not being supplied
  STNextTokenUsingDifferentDelimiters(st, dataBuffer, bufferLength, ">");
  RemoveEscapeCharacters(dataBuffer);
  char* valueContent;
  if (dataBuffer[0]=='<'&&dataBuffer[1]=='!'){
    valueContent=dataBuffer+9;
    int contentLength=strlen(valueContent);
    memmove(dataBuffer,valueContent,contentLength-2);
    dataBuffer[contentLength-2]='\0';
    STSkipOver(st, ">");
    STSkipUntil(st, ">");
    return;
  }
  else{
      dataBuffer[strlen(dataBuffer)-strlen(htmlTag)]='\0';
  }
  STSkipOver(st, ">");
}

/**
 * Function: ParseArticle
 * ----------------------
 * Attempts to establish a network connect to the news article identified by the three
 * parameters.  The network connection is either established of not.  The implementation
 * is prepared to handle a subset of possible (but by far the most common) scenarios,
 * and those scenarios are categorized by response code:
 *
 *    0 means that the server in the URL doesn't even exist or couldn't be contacted.
 *    200 means that the document exists and that a connection to that very document has
 *        been established.
 *    301 means that the document has moved to a new location
 *    302 also means that the document has moved to a new location
 *    4xx and 5xx (which are covered by the default case) means that either
 *        we didn't have access to the document (403), the document didn't exist (404),
 *        or that the server failed in some undocumented way (5xx).
 *
 * The are other response codes, but for the time being we're punting on them, since
 * no others appears all that often, and it'd be tedious to be fully exhaustive in our
 * enumeration of all possibilities.
 */

static void ParseArticle(const char *articleTitle, const char *articleURL,hashset* querySet,hashset* stopSet,
                                                   pthread_mutex_t *quereMut,pthread_mutex_t *stopWordMut)
{

  urlconnection urlconn;
  streamtokenizer st;
  urlconn.responseCode=200;
  char* commond=malloc(256);
  char fileName[128];
  char tmp[128];
  memcpy(tmp,articleTitle,strlen(articleTitle)+1);
  for(int i=0;i<strlen(tmp);i++){
      if(tmp[i]=='\'')
         tmp[i]='-';
    }
  sprintf(fileName," -o \'%s\'",tmp);

  commond[0]='\0';
  assert(commond!=NULL);
  commond=strcat(commond,"curl -x 127.0.0.1:9666 -s ");
  commond=strcat(commond,articleURL);
  commond=strcat(commond,fileName);
  printf("%s \n",commond);

  system(commond);
  urlconn.dataStream=fopen(tmp,"r+");

  switch (urlconn.responseCode) {
      case 0: printf("Unable to connect to \"%s\".  Domain name or IP address is nonexistent.\n", articleURL);
          break;
      case 200: printf("Scanning \"%s\" from \"%s\"\n", articleTitle, articleURL);
            STNew(&st, urlconn.dataStream, kTextDelimiters, false);
        ScanArticle(&st, articleTitle,articleURL,querySet,stopSet,quereMut,stopWordMut);
        STDispose(&st);
        break;
        /*
      case 301:
      case 302: // just pretend we have the redirected URL all along, though index using the new URL and not the old one...
      ParseArticle(articleTitle,urlconn.newUrl,querySet,stopSet);
      */
      break;
      //default: printf("Unable to pull \"%s\" from \"%s\". [Response code: %d] Punting...\n", articleTitle, u.serverName, urlconn.responseCode);

  }
  fclose(urlconn.dataStream);
  free(commond);
  //sprintf(fileName,"rm \'%s\'",articleTitle);
  //system(fileName);


}

/**
 * Function: ScanArticle
 * ---------------------
 * Parses the specified article, skipping over all HTML tags, and counts the numbers
 * of well-formed words that could potentially serve as keys in the set of indices.
 * Once the full article has been scanned, the number of well-formed words is
 * printed, and the longest well-formed word we encountered along the way
 * is printed as well.
 *
 * This is really a placeholder implementation for what will ultimately be
 * code that indexes the specified content.
 */

static void ScanArticle(streamtokenizer *st, const char *articleTitle, const char *articleURL,hashset* querySet,hashset* stopSet
                        ,pthread_mutex_t *quereMut,pthread_mutex_t *stopWordMut)
{

  char word[1024];


  while (STNextToken(st, word, sizeof(word))) {
    if (strcasecmp(word, "<") == 0) {
      char w1=fgetc(st->infile);
      char w2=fgetc(st->infile);
      ungetc (w2,st->infile);
      ungetc (w1,st->infile);
      SkipIrrelevantContent(st);
      if((w1=='p'&&w2=='>') || (w1=='h'&& (w2=='1'||w2=='2') ))
        while(STNextToken (st,word,sizeof(word))){
            if (strcasecmp(word, "<") == 0){
                char w1=fgetc(st->infile);
                char w2=fgetc(st->infile);
                char w3=fgetc(st->infile);
                ungetc (w3,st->infile);
                ungetc (w2,st->infile);
                ungetc (w1,st->infile);
                SkipIrrelevantContent(st); // in html-utls.h
                if((w1=='/'&&w2=='p'&&w3=='>') || (w1=='/'&&w2=='h'&& (w3=='1'||w3=='2') )) break;
              }
            else{
                RemoveEscapeCharacters(word);
                changeToLowCase (word);
                struct wordUnit dummy;
                dummy.word=word;
                //printf ("Into locking \n");
                pthread_mutex_lock(stopWordMut);
                if(HashSetLookup (stopSet,&dummy)!=NULL){
                    pthread_mutex_unlock(stopWordMut);
                    continue;
                  }
                pthread_mutex_unlock(stopWordMut);
                // Add parse here
                if(strlen(word)==1) continue;
                pthread_mutex_lock(quereMut);
                struct wordUnit* wordPosUnion=HashSetLookup (querySet,&dummy);
                pthread_mutex_unlock(quereMut);
                if(wordPosUnion==NULL){
                    wordPosUnion=malloc(sizeof(struct wordUnit));
                    wordPosUnion->word=malloc(strlen(word)+1);
                    memcpy(wordPosUnion->word,word,strlen(word)+1);
                    wordPosUnion->posList=malloc(sizeof(vector));
                    VectorNew (wordPosUnion->posList,sizeof(struct pos),NULL,5);
                    pthread_mutex_lock(quereMut);
                    HashSetEnter (querySet,wordPosUnion);
                    void* tmp=wordPosUnion;
                    wordPosUnion=HashSetLookup (querySet,wordPosUnion);
                    pthread_mutex_unlock(quereMut);
                    free(tmp);
                  }
                pthread_mutex_lock(quereMut);
                if(VectorLength (wordPosUnion->posList)==0 ||
                   strcmp(((struct pos*)VectorNth (wordPosUnion->posList,VectorLength (wordPosUnion->posList)-1))->title,articleTitle)!=0){
                    struct pos* signlePos=malloc(sizeof(struct pos));
                    signlePos->count=1;
                    signlePos->title=articleTitle;
                    signlePos->url=articleURL;
                    VectorAppend (wordPosUnion->posList,signlePos);
                    free(signlePos);
                  }
                else
                    ((struct pos*)VectorNth (wordPosUnion->posList,VectorLength (wordPosUnion->posList)-1))->count+=1;
                pthread_mutex_unlock(quereMut);
              }
          }
      }


  }

  /*
  printf("\tWe counted %d well-formed words [including duplicates].\n", numWords);
  printf("\tThe longest word scanned was \"%s\".", longestWord);
  if (strlen(longestWord) >= 15 && (strchr(longestWord, '-') == NULL))
    printf(" [Ooooo... long word!]");
  printf("\n");
  */
}

/**
 * Function: QueryIndices
 * ----------------------
 * Standard query loop that allows the user to specify a single search term, and
 * then proceeds (via ProcessResponse) to list up to 10 articles (sorted by relevance)
 * that contain that word.
 */

static void QueryIndices(hashset* querySet,hashset* stopSet)
{
  char response[1024];
  while (true) {
    printf("Please enter a single query term that might be in our set of indices [enter to quit]: ");
    fgets(response, sizeof(response), stdin);
    response[strlen(response) - 1] = '\0';
    if (strcasecmp(response, "") == 0) break;
    ProcessResponse(response,querySet,stopSet);
  }
}

/**
 * Function: ProcessResponse
 * -------------------------
 * Placeholder implementation for what will become the search of a set of indices
 * for a list of web documents containing the specified word.
 */

static void ProcessResponse(const char *word, hashset *querySet, hashset *stopSet)
{
  if(!WordIsWellFormed(word)){
      printf("\tWe won't be allowing words like \"%s\" into our set of indices.\n", word);
      return;
    }
  changeToLowCase (word);
  struct wordUnit* dummy=malloc(sizeof(struct wordUnit));
  dummy->word=word;
  if(HashSetLookup (stopSet,dummy)!=NULL){
      printf ("\tThe search term is so common or meaningless, try other words\n");
      free(dummy);
      return;
    }
  struct wordUnit* serachResult=HashSetLookup (querySet,dummy);
  free(dummy);
  if(serachResult==NULL)
    printf ("None of today's news articles contain the word \"%s\".\n",word);
  else{
      int articlesNum=VectorLength (serachResult->posList);
      assert(articlesNum>0);
      if(articlesNum<=10)
        printf ("Nice! We found %d articles that include the word \"%s\".\n\n",articlesNum,word);
      else
        printf ("We found %d articles with the word \"%s\". [We'll just list 10, though.]\n\n",articlesNum,word);
      for(int i=1;i<=articlesNum;i++){
          struct pos *elemPos=VectorNth (serachResult->posList,i-1);
          printf ("\t%d.)\t\"%s\" [search term occurs %d times]\n",i,elemPos->title,elemPos->count);
          printf ("\t    \t\"%s\"\n",elemPos->url);
        }
    }
  printf ("\n");
  return;
}

/**
 * Predicate Function: WordIsWellFormed
 * ------------------------------------
 * Before we allow a word to be inserted into our map
 * of indices, we'd like to confirm that it's a good search term.
 * One could generalize this function to allow different criteria, but
 * this version hard codes the requirement that a word begin with
 * a letter of the alphabet and that all letters are either letters, numbers,
 * or the '-' character.
 */

static bool WordIsWellFormed(const char *word)
{
  int i;
  if (strlen(word) == 0) return true;
  if (!isalpha((int) word[0])) return false;
  for (i = 1; i < strlen(word); i++)
    if (!isalnum((int) word[i]) && (word[i] != '-')) return false;

  return true;
}
