/***************************************************************************
 *   Copyright (C) 2010~2010 by CSSlayer                                   *
 *   wengxt@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <ime-core/imi_view.h>
#include <fcitx/ime.h>
#include <fcitx/instance.h>
#define BUF_SIZE 4096

struct FcitxSunpinyin;
class FcitxWindowHandler : public CIMIWinHandler
{
public:
    virtual void updatePreedit(const IPreeditString* ppd);
    virtual void updateCandidates(const ICandidateList* pcl);
    //virtual void updateStatus(int key, int value);
    virtual void commit(const TWCHAR* str);

    void SetOwner(FcitxSunpinyin* owner_) {
        owner = owner_;
    }

    bool commit_flag ; 
    bool candidate_flag ;

private:
    FcitxSunpinyin* owner;   
    TWCHAR front_src[BUF_SIZE];
    TWCHAR end_src[BUF_SIZE];
    TWCHAR input_src[BUF_SIZE];
    char preedit[BUF_SIZE];
};