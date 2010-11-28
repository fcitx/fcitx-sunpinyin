/*  Copyright (C) 2010~2010 by CSSlayer
    wengxt@gmail.com 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <ime-core/imi_view.h>
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

