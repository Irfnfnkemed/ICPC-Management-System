//
// Created by irfnfnkemed on 2022/11/10.
//

#include "ICPC.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

void ADDTEAM();

void START();

void SUBMIT();

void FLUSH();

void FREEZE();

void SCROLL();

void QUERY_RANKING();

void QUERY_SUBMISSION();

int problem_count, duration_time;
bool flag_freeze = false;
std::string command;

//记录某队某题目最新的信息
struct Problem {
    //各种最后上交的时间
    int last_ac = -1, last_re = -1, last_wa = -1, last_tle = -1;
    int first_ac = -1;//第一次通过时间
    int num_fail = 0;//第一次通过前错误次数
    char sta_last = 0;//最后一题状态
};

//记录某队的所有最新信息
struct Information {
    Problem Pro[30];//某队所有的题目
    int pro_time[30];//某队所有通过的时间
    int ac_num = 0, punish = 0;//某队通过题数和罚时
    //某队最后提交的状态的时间
    int pro_ac = -1, pro_wa = -1, pro_re = -1, pro_tle = -1, pro_all = -1;
} Team[10010];

//方便找到队伍名对应的信息
std::unordered_map<std::string, int> Number;

//记录某队封榜后的信息
struct F_Information {
    int f_ac_num, f_punish;//某队在封榜时的通过题数及罚时
    int f_pro_time[30];//某队在封榜时所有通过的时间
    int f_fail[30];//某题之前错误次数
    bool f_pass[30];//某题是否已通过
    bool f_freeze[30];//某题是否冻结
    int f_sub[30];//某题在封榜期间提交次数
} F_team[10010];

//排序
struct cmp {
    bool operator()(const std::string &lhs, const std::string &rhs) const {
        //封榜时，按F_team内信息排序，否则按Team内信息排序
        if (flag_freeze) {
            if (F_team[Number[lhs]].f_ac_num != F_team[Number[rhs]].f_ac_num) {
                return F_team[Number[lhs]].f_ac_num > F_team[Number[rhs]].f_ac_num;
            }
            if (F_team[Number[lhs]].f_punish != F_team[Number[rhs]].f_punish) {
                return F_team[Number[lhs]].f_punish < F_team[Number[rhs]].f_punish;
            }
            for (int i = F_team[Number[lhs]].f_ac_num; i >= 1; --i) {
                if (F_team[Number[lhs]].f_pro_time[i] != F_team[Number[rhs]].f_pro_time[i])
                    return F_team[Number[lhs]].f_pro_time[i] < F_team[Number[rhs]].f_pro_time[i];
            }
            return lhs < rhs;
        } else {//否则按Team内数据排序
            if (Team[Number[lhs]].ac_num != Team[Number[rhs]].ac_num) {
                return Team[Number[lhs]].ac_num > Team[Number[rhs]].ac_num;
            }
            if (Team[Number[lhs]].punish != Team[Number[rhs]].punish) {
                return Team[Number[lhs]].punish < Team[Number[rhs]].punish;
            }
            for (int i = Team[Number[lhs]].ac_num; i >= 1; --i) {
                if (Team[Number[lhs]].pro_time[i] != Team[Number[rhs]].pro_time[i])
                    return Team[Number[lhs]].pro_time[i] < Team[Number[rhs]].pro_time[i];
            }
            return lhs < rhs;
        }
    }
};

//实时更新(封榜除外)的榜单
std::set<std::string, cmp> List;

//记录队名
std::set<std::string> Name;

//刷新后保持不变的排名
std::unordered_map<std::string, int> Rank;


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    while (1) {
        std::cin >> command;
        if (command == "ADDTEAM") {//添加队伍
            ADDTEAM();
        }
        if (command == "START") {//开始比赛
            START();
            break;
        }
    }
    while (1) {
        std::cin >> command;
        if (command == "SUBMIT") {//提交题目
            SUBMIT();
        } else if (command == "FLUSH") {//刷新榜单
            FLUSH();
        } else if (command == "FREEZE") {//封榜
            FREEZE();
        } else if (command == "SCROLL") {//滚榜
            SCROLL();
        } else if (command == "QUERY_RANKING") {//查询队伍排名
            QUERY_RANKING();
        } else if (command == "QUERY_SUBMISSION") {//查询队伍提交情况
            QUERY_SUBMISSION();
        } else if (command == "ADDTEAM") {
            std::cin >> command;
            std::cout << "[Error]Add failed: competition has started.\n";
        } else if (command == "START") {
            std::cin >> command >> command >> command >> command;
            std::cout << "[Error]Start failed: competition has started.\n";
        } else if (command == "END") {
            std::cout << "[Info]Competition ends.\n";
            break;
        }
    }
    return 0;
}

void ADDTEAM() {
    std::string a_name;
    std::cin >> a_name;
    if (Name.count(a_name) == 1) {
        std::cout << "[Error]Add failed: duplicated team name.\n";
    } else {
        Name.insert(a_name);
        std::cout << "[Info]Add successfully.\n";
    }
}

void START() {
    std::cin >> command >> duration_time >> command >> problem_count;
    //初始化
    for (auto iter = Name.begin(); iter != Name.end(); ++iter) {
        static int start_rank = 1;
        Number[*iter] = Rank[*iter] = start_rank;
        List.insert(*iter);
        ++start_rank;
    }
    std::cout << "[Info]Competition starts.\n";
}

void SUBMIT() {
    char su_mark;
    std::string su_name, su_status;
    int su_time, su_pro;
    std::cin >> su_mark >> command >> su_name >> command
             >> su_status >> command >> su_time;
    su_pro = su_mark - 'A' + 1;
    //若已封榜，处理封榜有关信息
    if (flag_freeze) {
        //若提交的题之前未通过，冻结，否则不冻结
        if ((Team[Number[su_name]].Pro)[su_pro].last_ac == -1) {
            (F_team[Number[su_name]].f_freeze)[su_pro] = true;
        }
        //若已冻结，记录封榜期间提交次数
        if ((F_team[Number[su_name]].f_freeze)[su_pro]) {
            ++F_team[Number[su_name]].f_sub[su_pro];
        }
    } else { List.erase(su_name); }//未封榜，删去实时榜单中的该队伍，更新List
    Problem su_tmp = Team[Number[su_name]].Pro[su_pro];
    //更新s_tmp及Team中数据
    if (su_status == "Accepted") {
        //若之前未通过,更新罚时等
        if (su_tmp.last_ac == -1) {
            Team[Number[su_name]].punish += 20 * su_tmp.num_fail + su_time;
            ++Team[Number[su_name]].ac_num;
            Team[Number[su_name]].pro_time[Team[Number[su_name]].ac_num] = su_time;
            su_tmp.first_ac = su_time;
        }
        su_tmp.sta_last = 'A';
        su_tmp.last_ac = su_time;
        Team[Number[su_name]].pro_ac = Team[Number[su_name]].pro_all = su_pro;
    } else {
        //若之前未通过，增加错误次数
        if (su_tmp.last_ac == -1) {
            ++su_tmp.num_fail;
        }
        if (su_status == "Wrong_Answer") {
            su_tmp.sta_last = 'W';
            su_tmp.last_wa = su_time;
            Team[Number[su_name]].pro_wa = Team[Number[su_name]].pro_all = su_pro;
        } else if (su_status == "Runtime_Error") {
            su_tmp.sta_last = 'R';
            su_tmp.last_re = su_time;
            Team[Number[su_name]].pro_re = Team[Number[su_name]].pro_all = su_pro;
        } else if (su_status == "Time_Limit_Exceed") {
            su_tmp.sta_last = 'T';
            su_tmp.last_tle = su_time;
            Team[Number[su_name]].pro_tle = Team[Number[su_name]].pro_all = su_pro;
        }
    }
    //将数据添加回Team和List(未封榜)中
    (Team[Number[su_name]].Pro)[su_pro] = su_tmp;
    if (!flag_freeze) List.insert(su_name);
}

void FLUSH() {
    int fl_rank = 1;
    for (auto iter = List.begin(); iter != List.end(); ++iter) {
        Rank[*iter] = fl_rank;
        ++fl_rank;
    }
    std::cout << "[Info]Flush scoreboard.\n";
}

void FREEZE() {
    if (flag_freeze) std::cout << "[Error]Freeze failed: scoreboard has been frozen.\n";
    else {
        //初始化有关封榜的信息
        for (auto iter = Name.begin(); iter != Name.end(); ++iter) {
            for (int i = 1; i <= problem_count; ++i) {
                F_team[Number[*iter]].f_freeze[i] = false;
                F_team[Number[*iter]].f_sub[i] = 0;
                F_team[Number[*iter]].f_ac_num = Team[Number[*iter]].ac_num;
                F_team[Number[*iter]].f_punish = Team[Number[*iter]].punish;
                F_team[Number[*iter]].f_fail[i] = Team[Number[*iter]].Pro[i].num_fail;
                for (int i = 1; i <= Team[Number[*iter]].ac_num; ++i) {
                    F_team[Number[*iter]].f_pro_time[i] = Team[Number[*iter]].pro_time[i];
                }
                if (Team[Number[*iter]].Pro[i].last_ac == -1) {
                    F_team[Number[*iter]].f_pass[i] = false;
                } else { F_team[Number[*iter]].f_pass[i] = true; }

            }
        }
        flag_freeze = true;
        std::cout << "[Info]Freeze scoreboard.\n";
    }
}

void SCROLL() {
    if (flag_freeze) {
        std::cout << "[Info]Scroll scoreboard.\n";
        //要刷新榜单，即过程中用List(因为List中未更新封榜后的信息)
        //输出滚榜前的榜单
        int sc_rank = 1;
        for (auto iter = List.begin(); iter != List.end(); ++iter) {
            std::cout << *iter << " " << sc_rank;
            std::cout << " " << F_team[Number[*iter]].f_ac_num;
            std::cout << " " << F_team[Number[*iter]].f_punish;
            for (int i = 1; i <= problem_count; ++i) {
                //若已被冻结
                if (F_team[Number[*iter]].f_freeze[i]) {
                    if (F_team[Number[*iter]].f_fail[i] == 0) { std::cout << " 0"; }
                    else { std::cout << " -" << F_team[Number[*iter]].f_fail[i]; }
                    std::cout << "/" << F_team[Number[*iter]].f_sub[i];
                } else {
                    //未被冻结，但未通过
                    if (!F_team[Number[*iter]].f_pass[i]) {
                        if (F_team[Number[*iter]].f_fail[i] == 0) { std::cout << " ."; }
                        else { std::cout << " -" << F_team[Number[*iter]].f_fail[i]; }
                    } else {//未被冻结，但已通过
                        if (F_team[Number[*iter]].f_fail[i] == 0) { std::cout << " +"; }
                        else { std::cout << " +" << F_team[Number[*iter]].f_fail[i]; }
                    }
                }
            }
            std::cout << '\n';
            ++sc_rank;
        }
        //每一行输出滚榜时导致排名变化的解冻
        std::string sc_name;
        int sc_begin[10010];//榜单中每一队中可能未解冻的题目的最小编号
        for (int i = 1; i <= Name.size(); ++i)
            sc_begin[i] = 1;//初始化
        while (1) {
            bool sc_flag = true;
            auto iter = List.end();
            --iter;
            //List中只剩一元素时，直接退出循环，因为这是第一名，不影响后续
            if (iter == List.begin()) { break; }
            sc_name = *iter;
            for (int i = sc_begin[Number[sc_name]]; i <= problem_count; ++i) {
                if (F_team[Number[sc_name]].f_freeze[i]) {
                    //更新榜单中最后一队中可能未解冻的题目的最小编号
                    sc_begin[Number[sc_name]] = i + 1;
                    //若从不正确到正确，记录F_team中新正确题数和罚时等，并输出变化
                    if (!F_team[Number[sc_name]].f_pass[i] &&
                        Team[Number[sc_name]].Pro[i].last_ac != -1) {
                        List.erase(sc_name);//实现滚榜中的更新
                        ++F_team[Number[sc_name]].f_ac_num;
                        F_team[Number[sc_name]].f_punish +=
                                Team[Number[*iter]].Pro[i].first_ac +
                                Team[Number[*iter]].Pro[i].num_fail * 20;
                        F_team[Number[sc_name]].f_pro_time[F_team[Number[sc_name]].f_ac_num] = Team[Number[*iter]].Pro[i].first_ac;
                        for (int i = F_team[Number[sc_name]].f_ac_num; i > 1; --i)
                            if (F_team[Number[sc_name]].f_pro_time[i] <
                                F_team[Number[sc_name]].f_pro_time[i - 1]) {
                                int tmp_time = F_team[Number[sc_name]].f_pro_time[i - 1];
                                F_team[Number[sc_name]].f_pro_time[i - 1] =
                                        F_team[Number[sc_name]].f_pro_time[i];
                                F_team[Number[sc_name]].f_pro_time[i] = tmp_time;
                            } else { break; }
                        //重新插入，便于排序
                        List.insert(sc_name);
                        auto sc_iter = List.lower_bound(sc_name);
                        ++sc_iter;
                        if (sc_iter != List.end()) {
                            std::cout << sc_name << " " << *sc_iter << " ";
                            std::cout << F_team[Number[sc_name]].f_ac_num << " ";
                            std::cout << F_team[Number[sc_name]].f_punish << '\n';
                        }
                    }
                    F_team[Number[sc_name]].f_freeze[i] = false;//解冻
                    sc_flag = false;
                    break;
                }
            }
            //否则某队伍已全部解冻，删去，防止后续再次遍历造成浪费
            if (sc_flag) { List.erase(sc_name); }
        }
        flag_freeze = false;//改变封榜状态，便于排序
        //在List中添回所有队伍
        for (auto iter = Name.begin(); iter != Name.end(); ++iter) {
            List.insert(*iter);
        }
        //更新排名
        sc_rank = 1;
        for (auto iter = List.begin(); iter != List.end(); ++iter) {
            Rank[*iter] = sc_rank;
            ++sc_rank;
        }
        //输出滚榜完毕后的榜单
        for (auto iter = List.begin(); iter != List.end(); ++iter) {
            std::cout << *iter << " " << Rank[*iter];
            std::cout << " " << Team[Number[*iter]].ac_num;
            std::cout << " " << Team[Number[*iter]].punish;
            for (int i = 1; i <= problem_count; ++i) {
                if (Team[Number[*iter]].Pro[i].last_ac == -1) {
                    if (Team[Number[*iter]].Pro[i].num_fail == 0) { std::cout << " ."; }
                    else { std::cout << " -" << Team[Number[*iter]].Pro[i].num_fail; }
                } else {
                    if (Team[Number[*iter]].Pro[i].num_fail == 0) { std::cout << " +"; }
                    else { std::cout << " +" << Team[Number[*iter]].Pro[i].num_fail; }
                }
            }
            std::cout << '\n';
        }

    } else { std::cout << "[Error]Scroll failed: scoreboard has not been frozen.\n"; }
}

void QUERY_RANKING() {
    std::string qr_name;
    std::cin >> qr_name;
    if (Name.count(qr_name) == 0) {
        std::cout << "[Error]Query ranking failed: cannot find the team.\n";
    } else {
        std::cout << "[Info]Complete query ranking.\n";
        if (flag_freeze) {
            std::cout << "[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled.\n";
        }
        std::cout << qr_name << " NOW AT RANKING " << Rank[qr_name] << '\n';
    }
}

void QUERY_SUBMISSION() {
    std::string qs_name, qs_pro, qs_sta;
    std::cin >> qs_name >> command >> qs_pro >> command >> qs_sta;
    if (Name.count(qs_name) == 0) {
        std::cout << "[Error]Query submission failed: cannot find the team.\n";
    } else {
        std::cout << "[Info]Complete query submission.\n";
        int qs_time = -1;
        int qs_mark = -1;
        if (qs_pro == "PROBLEM=ALL") {
            //所有问题所有状态
            if (qs_sta == "STATUS=ALL") {
                if (Team[Number[qs_name]].pro_all != -1) {
                    qs_mark = Team[Number[qs_name]].pro_all;
                    char sta = Team[Number[qs_name]].Pro[qs_mark].sta_last;
                    if (sta == 'A') {
                        qs_time = Team[Number[qs_name]].Pro[qs_mark].last_ac;
                        qs_sta = "Accepted";
                    } else if (sta == 'W') {
                        qs_time = Team[Number[qs_name]].Pro[qs_mark].last_wa;
                        qs_sta = "Wrong_Answer";
                    } else if (sta == 'R') {
                        qs_time = Team[Number[qs_name]].Pro[qs_mark].last_re;
                        qs_sta = "Runtime_Error";
                    } else if (sta == 'T') {
                        qs_time = Team[Number[qs_name]].Pro[qs_mark].last_tle;
                        qs_sta = "Time_Limit_Exceed";
                    }
                }
            } else {
                //所有问题某些状态
                if (qs_sta == "STATUS=Accepted") {
                    qs_sta = "Accepted";
                    if (Team[Number[qs_name]].pro_ac != -1) {
                        qs_mark = Team[Number[qs_name]].pro_ac;
                        qs_time = Team[Number[qs_name]].Pro[qs_mark].last_ac;
                    }
                } else if (qs_sta == "STATUS=Wrong_Answer") {
                    qs_sta = "Wrong_Answer";
                    if (Team[Number[qs_name]].pro_wa != -1) {
                        qs_mark = Team[Number[qs_name]].pro_wa;
                        qs_time = Team[Number[qs_name]].Pro[qs_mark].last_wa;
                    }
                } else if (qs_sta == "STATUS=Runtime_Error") {
                    qs_sta = "Runtime_Error";
                    if (Team[Number[qs_name]].pro_re != -1) {
                        qs_mark = Team[Number[qs_name]].pro_re;
                        qs_time = Team[Number[qs_name]].Pro[qs_mark].last_re;
                    }
                } else if (qs_sta == "STATUS=Time_Limit_Exceed") {
                    qs_sta = "Time_Limit_Exceed";
                    if (Team[Number[qs_name]].pro_tle != -1) {
                        qs_mark = Team[Number[qs_name]].pro_tle;
                        qs_time = Team[Number[qs_name]].Pro[qs_mark].last_tle;
                    }
                }
            }
        } else {
            qs_mark = qs_pro[8] - 'A' + 1;
            //某问题所有状态
            if (qs_sta == "STATUS=ALL") {
                if (Team[Number[qs_name]].Pro[qs_mark].sta_last == 'A') {
                    qs_time = Team[Number[qs_name]].Pro[qs_mark].last_ac;
                    qs_sta = "Accepted";
                } else if (Team[Number[qs_name]].Pro[qs_mark].sta_last == 'W') {
                    qs_time = Team[Number[qs_name]].Pro[qs_mark].last_wa;
                    qs_sta = "Wrong_Answer";
                } else if (Team[Number[qs_name]].Pro[qs_mark].sta_last == 'R') {
                    qs_time = Team[Number[qs_name]].Pro[qs_mark].last_re;
                    qs_sta = "Runtime_Error";
                } else if (Team[Number[qs_name]].Pro[qs_mark].sta_last == 'T') {
                    qs_time = Team[Number[qs_name]].Pro[qs_mark].last_tle;
                    qs_sta = "Time_Limit_Exceed";
                }
            } else {
                //某些问题某些状态
                if (qs_sta == "STATUS=Accepted") {
                    qs_sta = "Accepted";
                    qs_time = Team[Number[qs_name]].Pro[qs_mark].last_ac;
                } else if (qs_sta == "STATUS=Wrong_Answer") {
                    qs_sta = "Wrong_Answer";
                    qs_time = Team[Number[qs_name]].Pro[qs_mark].last_wa;
                } else if (qs_sta == "STATUS=Runtime_Error") {
                    qs_sta = "Runtime_Error";
                    qs_time = Team[Number[qs_name]].Pro[qs_mark].last_re;
                } else if (qs_sta == "STATUS=Time_Limit_Exceed") {
                    qs_sta = "Time_Limit_Exceed";
                    qs_time = Team[Number[qs_name]].Pro[qs_mark].last_tle;
                }
            }
        }
        if (qs_time == -1) std::cout << "Cannot find any submission.\n";
        else {
            std::cout << qs_name << " ";
            if (qs_pro == "PROBLEM=ALL") std::cout << char(qs_mark + 'A' - 1);
            else std::cout << qs_pro[8];
            std::cout << " " << qs_sta << " " << qs_time << '\n';
        }
    }
}

///unordered_map  ！！！无序，查找平均只用O（1）
