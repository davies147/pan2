#include <config.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pan/general/test.h>
#include "text-massager.h"

using namespace pan;

int main (void)
{
   std::string in;
   std::string out;
   std::string expected_out;
   TextMassager tm;

   /* blank lines between quotes */
   tm.set_wrap_column (50);
   in = "> a\n\n> b";
   out = tm.fill (in);
   expected_out = "> a\n\n> b";
   check (out == expected_out);

   /* quoted paragraphs breaks */
   in = "> a\n>\n> b";
   out = tm.fill (in);
   expected_out = "> a\n>\n> b";
   check (out == expected_out);

   /* simple short quoted text - should be unchanged */
   in = "> a\n> b\n> c";
   out = tm.fill (in);
   expected_out = "> a\n> b\n> c";
   check (out == expected_out);

   /* wrap real-world 1 */
   in = 
"Cybe R. Wizard wrote:\n"
"\n"
"> Nice to know it works, right, and that's why I tried it.\n"
"> I ran SETI@home under win95 for a while but on my Pentium 166 it's not\n"
"> really worth it.  It took upwards of 500 hours to do one WU running\n"
"> full time in the background.\n"
"> Will the Linux version do better???\n"
"\n"
"500 hours seems like an awfully long time to me... I'm running setiathome \n"
"on all my systems, and on my P200's a work unit takes about 25-30 hours, \n"
"running as a low priority task with nice 19.\n"
"\n"
"> Here's a funny thing. Under the wine version that came with my\n"
"> Mandrake 7.2 the Galaxies 2.0 screensaver ran VERY slowly.  I had no\n"
"> real hope that Codeweaver's wine would do any better but the thing\n"
"> runs FASTER than under win95.\n"
"> I wonder why that is...\n"
"\n"
"Heh, I remember OS/2 running Windows programs faster than windows did :^)\n"
"\n"
"Or as I remarked to my wife this morning, as we were watching one of our \n"
"puppies amusing himself by crawling under our bed: \"Dogs crawl under \n"
"furniture.... Software crawls under windows\" :^)\n"
"\n"
"Jan Eric";
   expected_out = 
"Cybe R. Wizard wrote:\n"
"\n"
"> Nice to know it works, right, and that's why I\n"
"> tried it. I ran SETI@home under win95 for a\n"
"> while but on my Pentium 166 it's not really\n"
"> worth it.  It took upwards of 500 hours to do\n"
"> one WU running full time in the background. Will\n"
"> the Linux version do better???\n"
"\n"
"500 hours seems like an awfully long time to me...\n"
"I'm running setiathome on all my systems, and on\n"
"my P200's a work unit takes about 25-30 hours,\n"
"running as a low priority task with nice 19.\n"
"\n"
"> Here's a funny thing. Under the wine version\n"
"> that came with my Mandrake 7.2 the Galaxies 2.0\n"
"> screensaver ran VERY slowly.  I had no real hope\n"
"> that Codeweaver's wine would do any better but\n"
"> the thing runs FASTER than under win95. I wonder\n"
"> why that is...\n"
"\n"
"Heh, I remember OS/2 running Windows programs\n"
"faster than windows did :^)\n"
"\n"
"Or as I remarked to my wife this morning, as we\n"
"were watching one of our puppies amusing himself\n"
"by crawling under our bed: \"Dogs crawl under\n"
"furniture.... Software crawls under windows\" :^)\n"
"\n"
"Jan Eric";
   out = tm.fill (in);
   check (out == expected_out);

   /* wrap real-world 2 */
   in = 
"In article <bl0D6.3171$Uo2.75315@zwoll1.home.nl>, \"Marcel Pol\"\n"
"<mpol@nospam.gmx.net> wrote:\n"
"\n"
"> Recently \"Unknown\" <bill.m@no.spam.net> wrote:\n"
">> Knode is not for me\n"
">>     Question: What are the alternative apps. to Knode - especially in\n"
">>     off-line readers?\n"
"> \n"
"> I dunno any good kde newsreaders. I do like pan a lot. It's a gnome/gtk\n"
"> thing though. But if you don't care too much about a gtk thing in qyour\n"
"> kde-desktop, check out pan.\n"
"> \n"
"> Btw, you can let a kde-theme be applied to gtk programs too.  My gtk\n"
"> programs look just like kde, with it's default theme.\n"
"> \n"
"> \n"
"> --\n"
"> Marcel Pol mpol@mpol.dhs.org\n"
"> \n"
"> ...my cow ate the CDs.\n"
"\n"
"Pan has been going through a lot of modifications recently so make sure\n"
"you get the latest version you can run with your distro.\n";
   expected_out = 
"In article\n"
"<bl0D6.3171$Uo2.75315@zwoll1.home.nl>,\n"
"\"Marcel Pol\"\n"
"<mpol@nospam.gmx.net> wrote:\n"
"\n"
"> Recently \"Unknown\"\n"
"> <bill.m@no.spam.net> wrote:\n"
">> Knode is not for me\n"
">>     Question: What are the\n"
">>     alternative apps. to\n"
">>     Knode - especially in\n"
">>     off-line readers?\n"
"> \n"
"> I dunno any good kde\n"
"> newsreaders. I do like pan a\n"
"> lot. It's a gnome/gtk thing\n"
"> though. But if you don't\n"
"> care too much about a gtk\n"
"> thing in qyour kde-desktop,\n"
"> check out pan.\n"
"> \n"
"> Btw, you can let a kde-theme\n"
"> be applied to gtk programs\n"
"> too.  My gtk programs look\n"
"> just like kde, with it's\n"
"> default theme.\n"
"> \n"
"> \n"
"> --\n"
"> Marcel Pol mpol@mpol.dhs.org\n"
"> \n"
"> ...my cow ate the CDs.\n"
"\n"
"Pan has been going through a\n"
"lot of modifications recently\n"
"so make sure you get the\n"
"latest version you can run\n"
"with your distro.";
   tm.set_wrap_column (30);
   out = tm.fill (in);
   check (out == expected_out);


   /* rot13 */
   in = "Rot-13 started with rn, trn and similar newsreaders back in the mid-1980's.  It was common practice for a while for offending messages, and messages with some hint or disclosure (such as the answer to a question or puzzle posed in the message, or for covering spoilers to TV or movie episodes).";
   out = in;
   tm.rot13_inplace (const_cast<char*>(out.c_str()));
   expected_out = "Ebg-13 fgnegrq jvgu ea, gea naq fvzvyne arjfernqref onpx va gur zvq-1980'f.  Vg jnf pbzzba cenpgvpr sbe n juvyr sbe bssraqvat zrffntrf, naq zrffntrf jvgu fbzr uvag be qvfpybfher (fhpu nf gur nafjre gb n dhrfgvba be chmmyr cbfrq va gur zrffntr, be sbe pbirevat fcbvyref gb GI be zbivr rcvfbqrf).";
   check (out == expected_out);

   /* rot13 2 */
   tm.rot13_inplace (const_cast<char*>(out.c_str()));
   check (out == in);

   /* rot13 3 */
   in = "here is a line with a �,�,� but the line should not be truncated.";
   out = in;
   tm.rot13_inplace (const_cast<char*>(out.c_str()));
   expected_out = "urer vf n yvar jvgu n �,�,� ohg gur yvar fubhyq abg or gehapngrq.";
   check (out == expected_out);

   /* mute quoted test 2 */
   in =
"bill.m@no.spam.net wrote:\n"
"\n"
"> In <bl0D6.3171$Uo2.75315@zwoll1.home.nl>, on 04/17/01\n"
">    at 06:56 PM, \"Marcel Pol\" <mpol@nospam.gmx.net> said:\n"
"> \n"
"> .:.I do like pan a lot.\n"
"> .:.It's a gnome/gtk thing though.\n"
"> .:.But if you don't care too much about a gtk thing in qyour kde-desktop,\n"
"> check .:.out pan.\n"
"> \n"
"> Is this somewhere in mdk 7.2 (Complete)?\n"
"\n"
"pan is included with LM 7.2, but only version 0.81 - grab the 0.96 rpm from \n"
"the pan website instead.\n"
"\n"
"Jan Eric";
   expected_out = 
"bill.m@no.spam.net wrote:\n"
"\n"
"> [quoted text muted]\n"
"\n"
"pan is included with LM 7.2, but only version 0.81 - grab the 0.96 rpm from \n"
"the pan website instead.\n"
"\n"
"Jan Eric";
   out = tm.mute_quotes (in);
   check (out == expected_out);

   // mute quoted test: realworld 2
   check (out == expected_out);
   in =
"In article <bl0D6.3171$Uo2.75315@zwoll1.home.nl>, \"Marcel Pol\"\n"
"<mpol@nospam.gmx.net> wrote:\n"
"\n"
"> Recently \"Unknown\" <bill.m@no.spam.net> wrote:\n"
">> Knode is not for me\n"
">>     Question: What are the alternative apps. to Knode - especially in\n"
">>     off-line readers?\n"
"> \n"
"> I dunno any good kde newsreaders. I do like pan a lot. It's a gnome/gtk\n"
"> thing though. But if you don't care too much about a gtk thing in qyour\n"
"> kde-desktop, check out pan.\n"
"> \n"
"> Btw, you can let a kde-theme be applied to gtk programs too.  My gtk\n"
"> programs look just like kde, with it's default theme.\n"
"> \n"
"> \n"
"> --\n"
"> Marcel Pol mpol@mpol.dhs.org\n"
"> \n"
"> ...my cow ate the CDs.\n"
"\n"
"Pan has been going through a lot of modifications recently so make sure\n"
"you get the latest version you can run with your distro.";
   expected_out = 
"In article <bl0D6.3171$Uo2.75315@zwoll1.home.nl>, \"Marcel Pol\"\n"
"<mpol@nospam.gmx.net> wrote:\n"
"\n"
"> [quoted text muted]\n"
"\n"
"Pan has been going through a lot of modifications recently so make sure\n"
"you get the latest version you can run with your distro.";
   out = tm.mute_quotes (in);
   check (out == expected_out);

   // mute quoted text
   in = "> This is a bunch\n> of quoted text\n> which should be trimmed\n\nNot quoted.";
   expected_out = "> [quoted text muted]\n\nNot quoted.";
   out = tm.mute_quotes (in);
   check (out == expected_out);

   // mute quoted text
   in = "This is a bunch\nof nonquoted text\nwhich should be left alone\n\nNot quoted.";
   expected_out = in;
   out = tm.mute_quotes (in);
   check (out == expected_out);

   // success
   return 0;
}