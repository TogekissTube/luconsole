dnl Define macro for max tabs
AC_DEFUN([MAX_TABS], [10])

dnl Define a macro for get num for tabs
AC_DEFUN([GET_NUM_TABS], [AC_SUBST([NUM_TABS], [5])])

dnl New tab
AC_DEFUN([ADD_TAB], [
    AC_SUBST([TAB_TITLE], [$1])
    AC_SUBST([TAB_CONTENT], [$2])
])

dnl Title of Tab
AC_DEFUN([GET_TAB_TITLE], [AC_SUBST([TAB_TITLE], [$1])])

dnl Tab control
AC_DEFUN([GET_TAB_CONTENT], [AC_SUBST([TAB_CONTENT], [$1])])
