#ifndef __NTPW_H__
#define __NTPW_H__

/* Supported offline registry hives */
enum HIVE_ID {H_SAM=0, H_SEC, H_SOFT, /* Future: H_SYS, H_SOF, */ H_COUNT};

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
/* Returns 1 if the account appears to be a connected Microsoft account, 0 if not, -1 on error */
int is_ms_account(int rid);
/* Unlink a connected Microsoft account by setting a new local password.
 * Currently this simply installs a new password hash (making offline login possible)
 * and relies on Windows to treat it as local on next boot.
 */
int unlink_ms_account(int rid, char *password);

#endif /* __NTPW_H__*/
