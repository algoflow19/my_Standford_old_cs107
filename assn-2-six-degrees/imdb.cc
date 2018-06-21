using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

class ImdbCache{

  map<string,vector<film> > actorCredits;
  map<film,vector<string> > moiveCast;
private:
  friend imdb;

} imdbCache;



imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);

  int * tmp=(int*)actorFile;
  actorNum=*tmp;
  fileNum=* ((int*)movieFile);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const {
    if(player.length()<=1){
        cerr<<"Error player name got in getCredits"<<endl;
        exit(1);
    }
    if(imdbCache.actorCredits.count (player)!=0){
        films=imdbCache.actorCredits.at (player);
        return true;
      }

    int* actorBase=(int*)actorFile+1;
    int lo=0;
    int high=actorNum-1;
    int mid;
    while(true){
        mid=(lo+high)/2;
        short cmpResult=strcmp(
              (char*)getActorName(*(actorBase+mid))
              ,player.c_str ());

        if(cmpResult==0) break;
        if(cmpResult<0)
            lo=mid+1;
        else
            high=mid-1;
        if(lo>high){
            cerr<<"Not found the name in actorFile,Please check it out!"<<endl;
            return false;
        }
    }
    int actorOffset=actorBase[mid];
    if(loadActorInfo(actorOffset,films,player.length ())){
        imdbCache.actorCredits[player]=films;
        return true;
      }
    return false;
}

bool imdb::getCast(const film& movie, vector<string>& players)  const{
  if(imdbCache.moiveCast.count (movie)!=0){
      players=imdbCache.moiveCast[movie];
      return true;
    }
  film tmp;
  int *movieBase=(int *)movieFile+1;
  int lo=0;
  int hi=fileNum-1;
  int mid;
  while(true){
      mid=(lo+hi)/2;
      getMovieInfo (*(movieBase+mid),tmp);
      if(tmp==movie) break;
      if(tmp<movie)
        lo=mid+1;
      else
        hi=mid-1;
      if(lo>hi){
          cerr<<"Not found the name in actorFile,Please check it out!"<<endl;
          return false;
      }
    }
  if(loadFilmInfo (movieBase[mid],players,movie.title.length ())){
      imdbCache.moiveCast[movie]=players;
      return true;
    }
  return false;
}

string imdb::generateShorestPath(const string &s1, const string &s2)
{
  int pathLength=0;
  startSide.startPoint=s1;
  endSide.startPoint=s2;
  startSide.actorForProcess.push_front (s1);
  endSide.actorForProcess.push_front (s2);
  string tmp;
  while (true) {
      tmp=sideExecuteMove(startSide,endSide);
      if(tmp!="") return tmp;
      pathLength++;
      tmp=sideExecuteMove (endSide,startSide);
      if(tmp!="") return tmp;
      pathLength++;
      if(pathLength>5) return "";
    }

}

void imdb::printPath(const string &s1)
{
  const path & startPath=startSide.actorOnPath.at (s1);
  const vector<path::connection>& links=startPath.getLinks ();
  for(int i=0;i<links.size ();i++){
      if(i%2==0)
          cout<<"\t"<<links.at (i).player<<" was in \""<<links.at (i).movie.title<<"\" ("<<links.at (i).movie.year<<") with ";
      else
        cout<<links.at (i).player<<"."<<endl;
    }
  if(s1==endSide.startPoint) return;
  const path & endPath=endSide.actorOnPath.at (s1);
  const vector<path::connection>& endLinks=endPath.getLinks ();
  for(int i=0;i<endLinks.size ();i++){
      if(i%2==0)
        cout<<"\t"<<endLinks.at (endLinks.size ()-i-1).player<<
              " was in \""<<endLinks.at (endLinks.size ()-i-1).movie.title<<"\" ("<<endLinks.at (endLinks.size ()-i-1).movie.year
           <<") with ";
      else
        cout<<endLinks.at (endLinks.size ()-i-1).player<<"."<<endl;
    }
}

void imdb::clearPath()
{
  startSide.clearState ();
  endSide.clearState ();
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

string imdb::sideExecuteMove(finder &mainSide,finder& otherSide)
{
  vector<film> filmList;
  vector<string> actorList;
  int ProcessTimes=mainSide.actorForProcess.size ();
  for(string forProcessTarget : mainSide.actorForProcess)
    mainSide.processedActor.insert (forProcessTarget);
  for(int i=0;i<ProcessTimes;i++){
      string beginActor=mainSide.actorForProcess.back ();
      mainSide.actorForProcess.pop_back ();
      filmList.clear ();
      getCredits (beginActor,filmList);
      path &tmpPath=mainSide.actorOnPath[beginActor];
      for(film movie : filmList){
          if(mainSide.processedFilm.count (movie)!=0) continue;
          mainSide.processedFilm.insert (movie);
          actorList.clear ();
          getCast (movie,actorList);
          for(string actor : actorList){
              if(mainSide.processedActor.count (actor)!=0) continue;
              for(string possibleTarget : otherSide.actorForProcess)
                if(possibleTarget==actor){
                    mainSide.actorOnPath[actor]=tmpPath;
                    path &toAddedPath=mainSide.actorOnPath[actor];
                    toAddedPath.addConnection (movie,beginActor);
                    toAddedPath.addConnection (movie,actor);
                    return actor;
                  }
              mainSide.actorForProcess.push_front (actor);
              mainSide.actorOnPath[actor]=tmpPath;
              path &toAddedPath=mainSide.actorOnPath[actor];
              toAddedPath.addConnection (movie,beginActor);
              toAddedPath.addConnection (movie,actor);
            }
        }
      mainSide.actorOnPath.erase (beginActor);
    }
  return "";
}

bool imdb::loadFilmInfo(int offset, vector<string>& actorList,int movieNameLenght) const
{
  int actorArrayOffset=movieNameLenght+2;
  if(actorArrayOffset%2!=0) ++actorArrayOffset;
  short starActorNum=*(short*)((char*)movieFile+offset+actorArrayOffset);
  actorArrayOffset+=2;
  if(actorArrayOffset%4!=0) actorArrayOffset+=2;

  for(int i=0;i<starActorNum;i++){
      int actorOffset=*(int*)((char*)movieFile+offset+actorArrayOffset+4*i);
      actorList.push_back (getActorName (actorOffset));
    }
  return true;
}

bool imdb::loadActorInfo(int offset, vector<film> &filmList, int actorNameLength) const
{

  if(actorNameLength%2==0) actorNameLength++; // Add Pad
  short castMovieNum=*(short*)((char*)actorFile +offset+actorNameLength+1);
  short MoveiveArrayOffset=actorNameLength+1+2;
  if(MoveiveArrayOffset%4!=0) MoveiveArrayOffset+=2;
  for(int i=0;i<castMovieNum;i++){
      film tmp;
      getMovieInfo ( * (int *)((char*)actorFile+offset+MoveiveArrayOffset+i*4),tmp);
      filmList.push_back (tmp);
    }
  return true;
}

char *imdb::getActorName(int offset) const
{
  return (char*)actorFile+offset;
}

void imdb::getMovieInfo(int offset, film &container) const
{

  container.title=(char*)movieFile+offset;
  container.year=* ((char*)movieFile+offset+container.title.length ()+1) + 1900;
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}

void finder::clearState()
{
  while(!actorForProcess.empty ())
    actorForProcess.pop_back ();
  processedActor.clear ();
  processedFilm.clear ();
  actorOnPath.clear ();
}
