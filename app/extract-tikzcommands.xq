(:
 :   Copyright (C) 2010 by Glad Deschrijver
 :     <glad.deschrijver@gmail.com>
 :
 :   This program is free software; you can redistribute it and/or modify
 :   it under the terms of the GNU General Public License as published by
 :   the Free Software Foundation; either version 2 of the License, or
 :   (at your option) any later version.
 :
 :   This program is distributed in the hope that it will be useful,
 :   but WITHOUT ANY WARRANTY; without even the implied warranty of
 :   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 :   GNU General Public License for more details.
 :
 :   You should have received a copy of the GNU General Public License
 :   along with this program; if not, see <http://www.gnu.org/licenses/>.
 :)

(:
 : XQuery script to extract all translatable strings from tikzcommands.xml
 : and obtain a .h file containing QT_TRANSLATE_NOOP("TikzCommandInserter", str) lines.
 : The translatable strings are the arguments of <section title="...">,
 : <item name="..."> and the options which are the strings between &lt; and &gt;
 : in the arguments of <item description="...">.
 :)

declare variable $prefix := string("QT_TRANSLATE_NOOP(&quot;TikzCommandInserter&quot;, &quot;");
declare variable $suffix := concat("&quot;)", codepoints-to-string(10));

(: The function below gets the list of options of one <item> in the xml file :)

declare function local:getOption($text as xs:string)
{
    let $text := fn:replace($text, "&lt;=", "")
    let $option := fn:replace($text, ".*&lt;([^&gt;]*)&gt;.*", "\\1")
    let $option := fn:replace($option, " ", "&lt;") (: dirty hack to keep the spaces in an option, since we have filtered out all &lt; this is a good temporary replacement string :)
    return $option,

    (: get recursively the other options on the same line :)
    let $temp := fn:replace($text, "^[^&gt;]*", "")
    let $temp := fn:replace($temp, "^&gt;", "")
    return if (fn:contains($temp, "&lt;"))
    then
        local:getOption($temp)
    else ""
};

declare variable $doc := doc("tikzcommands.xml");

(: The function below gets the list of options of all <item>s in the xml file :)

declare function local:getOptionList()
{
    for $text in ($doc//item/@description)
        return if (fn:contains($text, "&lt;"))
        then
            let $options := fn:string-join(local:getOption($text), ' ')
            return $options
        else ""
};

(: join all options in a space-separated list which is then turned into a
 : real list from which all duplicates are removed :)

let $options := fn:string-join(local:getOptionList(), '')
let $options := fn:tokenize($options, '\s')
let $options := fn:distinct-values($options)
for $option in $options
    let $option := fn:replace($option, "&lt;", " ") (: dirty hack to keep the spaces in an option :)
    return fn:concat($prefix, $option, $suffix),

(: get the titles of the <section>s and the names of the <item>s :)

for $text in ($doc/tikzcommands/@title, $doc//section/@title, $doc//item/@name)
    let $text := fn:replace($text, '\\', '\\\\')
    let $text := fn:replace($text, '"', '\\"')
    return fn:concat($prefix, data($text), $suffix)
