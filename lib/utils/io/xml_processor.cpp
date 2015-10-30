/**
 * @brief Implementation of functions for parsing and printing XML documents.
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/xml_processor.hpp"

#include <assert.h>

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/io/xml_document.hpp"
#include "utils/string/string.hpp"

namespace genesis {

// =================================================================================================
//     Parsing
// =================================================================================================

/*
std::string XmlProcessor::XmlDescape (std::string& xml)
{
    std::string res;
    res = string_replace_all(xml, "&lt;",   "<");
    res = string_replace_all(res, "&gt;",   ">");
    res = string_replace_all(res, "&amp;",  "&");
    res = string_replace_all(res, "&apos;", "'");
    res = string_replace_all(res, "&quot;", "\"");
    return res;
}
*/

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Writes an XML file from an XmlDocument. Returns true iff successful.
 */
bool XmlProcessor::to_file (const std::string& fn, const XmlDocument& document)
{
    if (file_exists(fn)) {
        LOG_WARN << "XML file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string xml;
    to_string(xml, document);
    return file_write(fn, xml);
}

/**
 * @brief Gives the XML string representation of a XmlDocument.
 */
void XmlProcessor::to_string (std::string& xml, const XmlDocument& document)
{
    xml = to_string(document);
}

/**
 * @brief Returns the XML representation of a XmlDocument.
 */
std::string XmlProcessor::to_string (const XmlDocument& document)
{
    std::string res = "";
    if (!document.xml_tag.empty() || !document.declarations.empty()) {
        res = "<?" + document.xml_tag + print_attributes_list(document.declarations) + "?>\n";
    }
    print_element(res, &document, 0);
    return res + "\n";
}

/**
 * @brief Prints an XML comment.
 */
void XmlProcessor::print_comment (std::string& xml, const XmlComment* value)
{
    xml += "<!--" + value->content + "-->";
}

/**
 * @brief Prints an XML markup (simple text).
 */
void XmlProcessor::print_markup  (std::string& xml, const XmlMarkup*  value)
{
    xml += xml_escape(value->content);
}

/**
 * @brief Prints an XML element.
 */
void XmlProcessor::print_element (std::string& xml, const XmlElement* value, const int indent_level)
{
    // Prepare indention and opening tag.
    std::string in0 (indent_level * indent, ' ');
    xml += in0 + "<" + value->tag + print_attributes_list(value->attributes);

    // If it's an empty element, close it, and we are done.
    if (value->content.size() == 0) {
        xml += " />";
        return;
    }

    // If the element only contains a single markup, don't add new lines. However, if it contains
    // more data, put each element in a new line.
    xml += ">";
    if (value->content.size() == 1 && value->content[0]->is_markup()) {
        print_markup(xml, xml_value_to_markup(value->content[0].get()));
    } else {
        std::string in1 ((indent_level + 1) * indent, ' ');
        xml += "\n";

        for (auto& v : value->content) {
            if (v->is_comment()) {
                xml += in1;
                print_comment(xml, xml_value_to_comment(v.get()));
            } else if (v->is_markup()) {
                xml += in1;
                print_markup(xml, xml_value_to_markup(v.get()));
            } else if (v->is_element()) {
                print_element(xml, xml_value_to_element(v.get()), indent_level + 1);
            } else {
                // there are no other cases
                assert(0);
            }
            xml += "\n";
        }
        xml += in0;
    }

    xml += "</" + value->tag + ">";
}

/**
 * @brief Prints a list of XML attributes.
 */
std::string XmlProcessor::print_attributes_list (StringMapType attr)
{
    std::string xml;
    for (auto pair : attr) {
        xml += " " + pair.first + "=\"" + pair.second + "\"";
    }
    return xml;
}

/**
 * @brief Escape special XML characters.
 */
std::string XmlProcessor::xml_escape (const std::string& txt)
{
    std::string res;
    res = string_replace_all(txt, "<",  "&lt;");
    res = string_replace_all(res, ">",  "&gt;");
    res = string_replace_all(res, "&",  "&amp;");
    res = string_replace_all(res, "'",  "&apos;");
    res = string_replace_all(res, "\"", "&quot;");
    return res;
}

} // namespace genesis
