#ifndef __NTPW_H__
#define __NTPW_H__

enum HIVE_ID {H_SAM=0, /*H_SYS, H_SEC, H_SOF,*/ H_COUNT};

struct user_info
    {
    int rid;
    char *unicode_name;
    };

struct search_user
    {
    int count;
    int countri;
    int nkofs;
    };

int open_hives(char *fname[H_COUNT]);
void close_hives(void);
int is_hives_dirty(void);
int is_hives_ro(void);
int write_hives(void);
struct user_info *first_user(struct search_user *su);
struct user_info *next_user(struct search_user *su);
int is_account_locked(int rid);
int unlock_account(int rid);
int change_password(int rid, char *password);

#endif /* __NTPW_H__*/
