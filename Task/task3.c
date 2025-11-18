#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef enum
{
    GRUB_ERR_NONE = 0,
    GRUB_ERR_NO_DEP,
    GRUB_ERR_OUT_OF_MEMORY,
    GRUB_ERR_BAD_ARGUMENT,
    GRUB_ERR_BAD_MODULE,
}
grub_err_t;

typedef struct grub_dl_dep *grub_dl_dep_t;

// linked list for grub module
struct grub_dl
{
    char *name; // module name
    int ref_count; // how many modules depends on it (directly and indirectly, including itself)
    grub_dl_dep_t dep; // the linked list of other modules depending on it.
    struct grub_dl *next; // next modules
};

// node of dependencies linked list
struct grub_dl_dep
{
    struct grub_dl_dep *next;  // next node
    struct grub_dl *mod;  // pointer to modules
};

typedef struct grub_dl *grub_dl_t;

// linkd list head
grub_dl_t grub_dl_head = NULL;


/**
 * find and return the modules with name in the linkd list
 *
 * @param name the modules to find
 * @return pointer to modles or NULL if not find
 */
static inline grub_dl_t
grub_dl_get (const char *name)
{
    grub_dl_t l;

    for (l = grub_dl_head; l; l = l->next)
        if (strcmp (name, l->name) == 0)
            return l;

    return NULL;
}

/**
 * Increase ref_count of current depended moudles,
 * and recursive to all indirectly dependencies.
 * Invoked during a new moudles loading and refer a moudle as depended.
 *
 * @param mod the modules to increase
 * @return increased ref_count.
 */
int
grub_dl_ref (grub_dl_t mod)
{
    grub_dl_dep_t dep;

    for (dep = mod->dep; dep; dep = dep->next)
        grub_dl_ref (dep->mod);

    return ++mod->ref_count;
}

/**
 * Decrease ref_count of current depended moudles,
 * and recursive to all indirectly dependencies
 * Invoked during a new modles unloading.
 *
 * @param mod the modules to decrease
 * @return decreased ref_count.
 */

int
grub_dl_unref (grub_dl_t mod)
{
    grub_dl_dep_t dep;

    for (dep = mod->dep; dep; dep = dep->next)
        grub_dl_unref (dep->mod);

    return --mod->ref_count;
}

/**
 * Resolve the dependencies of a new loading module.
 * For every name of moduls:
 *  1. Find the moduls in the linked list.
 *  2. Increase the ref_count of moduls since we loaded a new moduls.
 *  3. Insert a new node in dependencies linked list.
 *
 * @param mod new loading the module
 * @param deps the name of depends list, splitd by "\0" and ended by a empty name
 * @return GRUB_ERR_NONE if everythiny done.
 *         GRUB_ERR_NO_DEP if any dependency can't be found.
 *         GRUB_ERR_OUT_OF_MEMORY if malloc failed.
 */
static grub_err_t
grub_dl_resolve_dependencies(grub_dl_t mod, char *deps)
{
    while (*deps)
    {
        grub_dl_t m;
        grub_dl_dep_t dep;

        m = grub_dl_get (deps);
        if (! m)
            return GRUB_ERR_NO_DEP;

        grub_dl_ref (m);

        dep = (grub_dl_dep_t) malloc (sizeof (*dep));
        if (! dep)
            return GRUB_ERR_OUT_OF_MEMORY;

        dep->mod = m;
        dep->next = mod->dep;
        mod->dep = dep;

        deps += strlen (deps) + 1;
    }

    return GRUB_ERR_NONE;
}


/**
 * Remove a node in linked list
 *
 * @param mod the node to remove
 */
static void
grub_dl_remove (grub_dl_t mod)
{
    grub_dl_t *p, q;

    for (p = &grub_dl_head, q = *p; q; p = &q->next, q = *p)
        if (q == mod)
        {
            *p = q->next;
            return;
        }
}


/**
 * Unload a module:
 *  1. check if ref_count == 1
 *  2. remove node in linked list
 *  3. for every module in dep linked list, try to unload it and free the  dep linked list node.
 *  3. free the module itself.
 *
 * @param mod the module node to remove
 * @return 1 if successful unload, 0 if its ref_count > 0 so can't be unloaded.
 */
int
grub_dl_unload (grub_dl_t mod)
{
    grub_dl_dep_t dep, depn;

    if (mod->ref_count > 0)
        return 0;

    grub_dl_remove (mod);

    for (dep = mod->dep; dep; dep = depn)
    {
        depn = dep->next;
        grub_dl_unload (dep->mod);
        free (dep);
    }

    free (mod->name);
    free (mod);
    return 1;
}


/**
 * Creates a duplicate of a string by allocating memory for the copy with malloc
 *
 * @param str the string to copy
 * @return new duplicate string
 */
char *
xstrdup (const char *str)
{
    size_t len;
    char *newstr;

    len = strlen (str);
    newstr = (char *) malloc (len + 1);
    memcpy (newstr, str, len + 1);

    return newstr;
}

/**
 * Command function: remove module
 *
 * @param name the module name to remove
 * @return 0 if successful remove, GRUB_ERR_BAD_ARGUMENT if can't find the name.
 */
static grub_err_t
grub_mini_cmd_rmmod (char *name)
{
    grub_dl_t mod;

    mod = grub_dl_get (name);
    if (! mod)
        return GRUB_ERR_BAD_ARGUMENT;

    if (grub_dl_unref (mod) <= 0)
        grub_dl_unload (mod);
    else
        printf("Warning: Can't rm %s: some mods depend on it\n", name);
    return 0;
}


/**
 * Command function: insert a module
 *
 * @param name the module name to insert
 * @param deps the name of depends list, splitd by "\0" and ended by a empty name
 * @return 0 if successful remove, GRUB_ERR_BAD_MODULE if the module already exist, GRUB_ERR_NO_DEP if any denpends doesn't exist
 */
static grub_err_t
grub_mini_cmd_insmod (char *name, char *deps)
{
    grub_dl_t mod;

    mod = grub_dl_get (name);
    if (mod)
        return GRUB_ERR_BAD_MODULE;

    mod = (grub_dl_t)malloc(sizeof (*mod));
    bzero(mod, sizeof (*mod));

    mod->name = xstrdup(name);
    mod->ref_count = 1;

    if (grub_dl_resolve_dependencies(mod, deps))
    {
        grub_dl_unload (mod);
        return GRUB_ERR_NO_DEP;
    }

    mod->next = grub_dl_head;
    grub_dl_head = mod;

    return 0;
}

/**
 * Command function: list all modules
 *
 * @return 0
 */
static grub_err_t
grub_mini_cmd_lsmod ()
{
    grub_dl_t mod;

    /* TRANSLATORS: this is module list header.  Name
       is module name, Ref Count is a reference counter
       (how many modules or open descriptors use it).
       Dependencies are the other modules it uses.
     */
    printf("Name\tRef Count\tDependencies\n");
    for (mod = grub_dl_head; mod; mod = mod->next)
    {
        grub_dl_dep_t dep;

        printf ("%s\t%d\t\t", mod->name, mod->ref_count);
        for (dep = mod->dep; dep; dep = dep->next)
        {
            if (dep != mod->dep)
                putchar (',');

            printf ("%s", dep->mod->name);
        }
        putchar ('\n');
    }

    return 0;
}

/**
 * Initial four moduels "A","B","C","D"
 * For command I insert a new modelus, input the name and deps.
 * For command D remove a modelus with name.
 * After each command, print the list of modelus.
 */
int main()
{
    grub_mini_cmd_insmod("A","\0");
    grub_mini_cmd_insmod("B","A\0\0");
    grub_mini_cmd_insmod("C","A\0\0");
    grub_mini_cmd_insmod("D","B\0C\0\0");

    grub_mini_cmd_lsmod();
    char buff[2], buff2[10];
    buff[1] = '\0';
    buff2[9] = '\0';
    char c;

    while (1) {
        c = getchar();
        if (c == 'I') {
            read(0, buff, 1);
            read(0, buff2, 9);
            grub_mini_cmd_insmod(buff, buff2);
        }
        else if (c == 'D') {
            buff[0] = getchar();
            grub_mini_cmd_rmmod(buff);
        } else
            break;
        grub_mini_cmd_lsmod();
    }

    return 0;
}
