#include <sunpinyin.h>
#include <fcitx/im.h>
#define BUF_SIZE 4096

class FcitxWindowHandler : public CIMIWinHandler
{
public:
    virtual void updatePreedit(const IPreeditString* ppd);
    virtual void updateCandidates(const ICandidateList* pcl);
    //virtual void updateStatus(int key, int value);
    virtual void commit(const TWCHAR* str);

    void set_eim(EXTRA_IM* eim_) {
        eim = eim_;
    }

    bool commit_flag ; 
    bool candidate_flag ;

private:
    EXTRA_IM* eim;   
    TWCHAR front_src[BUF_SIZE];
    TWCHAR end_src[BUF_SIZE];
};

