/**
* @author: Mital Vora <mital.d.vora@gmail.com>
*/

#ifndef _OPENXMPP_EXPORTS_H_
#define _OPENXMPP_EXPORTS_H_

# ifdef OPENXMPP_EXPORTS
#  define OPENXMPP_API __declspec(dllexport)
# else
#  define OPENXMPP_API __declspec(dllimport)
# endif
# define EXPORT __declspec(dllexport)

#endif
