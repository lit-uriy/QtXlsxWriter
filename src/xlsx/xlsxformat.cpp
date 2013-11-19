/****************************************************************************
** Copyright (c) 2013 Debao Zhang <hello@debao.me>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#include "xlsxformat.h"
#include "xlsxformat_p.h"
#include <QDataStream>
#include <QRegularExpression>
#include <QDebug>

QT_BEGIN_NAMESPACE_XLSX

FormatPrivate::FormatPrivate()
{
    dirty = true;

    is_dxf_fomat = false;
    xf_index = -1;
    dxf_index = -1;
    xf_indexValid = false;
    dxf_indexValid = false;

    theme = 0;
}

FormatPrivate::FormatPrivate(const FormatPrivate &other)
    : QSharedData(other)
    , numberData(other.numberData), fontData(other.fontData), alignmentData(other.alignmentData)
    , borderData(other.borderData), fillData(other.fillData), protectionData(other.protectionData)
    , dirty(other.dirty), formatKey(other.formatKey)
    , xf_index(other.xf_index), xf_indexValid(other.xf_indexValid)
    , is_dxf_fomat(other.is_dxf_fomat), dxf_index(other.dxf_index), dxf_indexValid(other.dxf_indexValid)
    , theme(other.theme)
{

}

FormatPrivate::~FormatPrivate()
{

}

/*!
 * \class Format
 * \inmodule QtXlsx
 * \brief Providing the methods and properties that are available for formatting cells in Excel.
 */


/*!
 *  Creates a new format.
 */
Format::Format() :
    d(new FormatPrivate)
{

}

/*!
   Creates a new format with the same attributes as the \a other format.
 */
Format::Format(const Format &other)
    :d(other.d)
{

}

/*!
   Assigns the \a other format to this format, and returns a
   reference to this format.
 */
Format &Format::operator =(const Format &other)
{
    d = other.d;
    return *this;
}

/*!
 * Destroys this format.
 */
Format::~Format()
{
}

/*!
 * Returns the number format identifier.
 */
int Format::numberFormatIndex() const
{
    return d->numberData.formatIndex;
}

/*!
 * Set the number format identifier. The \a format
 * must be a valid built-in number format identifier
 * or the identifier of a custom number format.
 */
void Format::setNumberFormatIndex(int format)
{
    d->dirty = true;
    d->numberData.formatIndex = format;
    d->numberData._valid = true;
}

/*!
 * Returns the number format string.
 * \note for built-in number formats, this may
 * return an empty string.
 */
QString Format::numberFormat() const
{
    return d->numberData.formatString;
}

/*!
 * Set number \a format.
 * http://office.microsoft.com/en-001/excel-help/create-a-custom-number-format-HP010342372.aspx
 */
void Format::setNumberFormat(const QString &format)
{
    if (format.isEmpty())
        return;
    d->dirty = true;
    d->numberData.formatString = format;
    d->numberData._valid = false; //formatIndex must be re-generated
}

/*!
 * Returns whether the number format is probably a dateTime or not
 */
bool Format::isDateTimeFormat() const
{
    if (d->numberData._valid && d->numberData.formatString.isEmpty()) {
        int idx = d->numberData.formatIndex;
        //Built in date time number index
        if ((idx >= 15 && idx <= 22) || (idx >= 45 && idx <= 47))
            return true;
    } else {
        //Gauss from the number string
        QString formatCode = d->numberData.formatString;
        formatCode.remove(QRegularExpression(QStringLiteral("\\[(Green|White|Blue|Magenta|Yellow|Cyan|Red)\\]")));
        if (formatCode.contains(QRegularExpression(QStringLiteral("[dmhys]"))))
            return true;
    }
    return false;
}

/*!
 * \internal
 */
bool Format::numFmtIndexValid() const
{
    return d->numberData._valid;
}

/*!
 * \internal
 */
void Format::setNumFmt(int index, const QString &string)
{
    d->numberData.formatIndex = index;
    d->numberData.formatString = string;
    d->numberData._valid = true;
}

/*!
 * Return the size of the font in points.
 */
int Format::fontSize() const
{
    return d->fontData.size;
}

/*!
 * Set the \a size of the font in points.
 */
void Format::setFontSize(int size)
{
    d->fontData.size = size;
    d->fontData._dirty = true;
}

/*!
 * Return whether the font is italic.
 */
bool Format::fontItalic() const
{
    return d->fontData.italic;
}

/*!
 * Turn on/off the italic font.
 */
void Format::setFontItalic(bool italic)
{
    d->fontData.italic = italic;
    d->fontData._dirty = true;
}

/*!
 * Return whether the font is strikeout.
 */
bool Format::fontStrikeOut() const
{
    return d->fontData.strikeOut;
}

/*!
 * Turn on/off the strikeOut font.
 */
void Format::setFontStrikeOut(bool strikeOut)
{
    d->fontData.strikeOut = strikeOut;
    d->fontData._dirty = true;
}

/*!
 * Return the color of the font.
 */
QColor Format::fontColor() const
{
    if (!d->fontData.color.isValid() && !d->fontData.themeColor.isEmpty()) {
        //!Todo, get the real color from the theme{1}.xml file
        //The same is ture for border and fill colord
        return QColor();
    }
    return d->fontData.color;
}

/*!
 * Set the \a color of the font.
 */
void Format::setFontColor(const QColor &color)
{
    d->fontData.color = color;
    d->fontData._dirty = true;
}

/*!
 * Return whether the font is bold.
 */
bool Format::fontBold() const
{
    return d->fontData.bold;
}

/*!
 * Turn on/off the bold font.
 */
void Format::setFontBold(bool bold)
{
    d->fontData.bold = bold;
    d->fontData._dirty = true;
}

/*!
 * Return the script style of the font.
 */
Format::FontScript Format::fontScript() const
{
    return d->fontData.scirpt;
}

/*!
 * Set the script style of the font.
 */
void Format::setFontScript(FontScript script)
{
    d->fontData.scirpt = script;
    d->fontData._dirty = true;
}

/*!
 * Return the underline style of the font.
 */
Format::FontUnderline Format::fontUnderline() const
{
    return d->fontData.underline;
}

/*!
 * Set the underline style of the font.
 */
void Format::setFontUnderline(FontUnderline underline)
{
    d->fontData.underline = underline;
    d->fontData._dirty = true;
}

/*!
 * Return whether the font is outline.
 */
bool Format::fontOutline() const
{
    return d->fontData.outline;
}

/*!
 * Turn on/off the outline font.
 */
void Format::setFontOutline(bool outline)
{
    d->fontData.outline = outline;
    d->fontData._dirty = true;
}

/*!
 * Return the name of the font.
 */
QString Format::fontName() const
{
    return d->fontData.name;
}

/*!
 * Set the name of the font.
 */
void Format::setFontName(const QString &name)
{
    d->fontData.name = name;
    d->fontData._dirty = true;
}

/*!
 * \internal
 */
bool Format::fontIndexValid() const
{
    return d->fontData.indexValid();
}

/*!
 * \internal
 */
int Format::fontIndex() const
{
    return d->fontData.index();
}

/*!
 * \internal
 */
void Format::setFontIndex(int index)
{
    d->fontData.setIndex(index);
}

/*!
 * \internal
 */
int Format::fontFamily() const
{
    return d->fontData.family;
}

/*!
 * \internal
 */
bool Format::fontShadow() const
{
    return d->fontData.shadow;
}

/*!
 * \internal
 */
QString Format::fontScheme() const
{
    return d->fontData.scheme;
}

/* Internal
 */
QByteArray Format::fontKey() const
{
    if (d->fontData._dirty)
        d->dirty = true; //Make sure formatKey() will be re-generated.
    return d->fontData.key();
}

/*!
 * Return the horizontal alignment.
 */
Format::HorizontalAlignment Format::horizontalAlignment() const
{
    return d->alignmentData.alignH;
}

/*!
 * Set the horizontal alignment.
 */
void Format::setHorizontalAlignment(HorizontalAlignment align)
{
    if (d->alignmentData.indent &&(align != AlignHGeneral && align != AlignLeft &&
                              align != AlignRight && align != AlignHDistributed)) {
        d->alignmentData.indent = 0;
    }

    if (d->alignmentData.shinkToFit && (align == AlignHFill || align == AlignHJustify
                                   || align == AlignHDistributed)) {
        d->alignmentData.shinkToFit = false;
    }

    d->alignmentData.alignH = align;
    d->dirty = true;
}

/*!
 * Return the vertical alignment.
 */
Format::VerticalAlignment Format::verticalAlignment() const
{
    return d->alignmentData.alignV;
}

/*!
 * Set the vertical alignment.
 */
void Format::setVerticalAlignment(VerticalAlignment align)
{
    d->alignmentData.alignV = align;
    d->dirty = true;
}

/*!
 * Return whether the cell text is wrapped.
 */
bool Format::textWrap() const
{
    return d->alignmentData.wrap;
}

/*!
 * Enable the text wrap
 */
void Format::setTextWarp(bool wrap)
{
    if (wrap && d->alignmentData.shinkToFit)
        d->alignmentData.shinkToFit = false;

    d->alignmentData.wrap = wrap;
    d->dirty = true;
}

/*!
 * Return the text rotation.
 */
int Format::rotation() const
{
    return d->alignmentData.rotation;
}

/*!
 * Set the text roation. Must be in the range [0, 180] or 255.
 */
void Format::setRotation(int rotation)
{
    d->alignmentData.rotation = rotation;
    d->dirty = true;
}

/*!
 * Return the text indentation level.
 */
int Format::indent() const
{
    return d->alignmentData.indent;
}

/*!
 * Set the text indentation level. Must be less than or equal to 15.
 */
void Format::setIndent(int indent)
{
    if (indent && (d->alignmentData.alignH != AlignHGeneral
                   && d->alignmentData.alignH != AlignLeft
                   && d->alignmentData.alignH != AlignRight
                   && d->alignmentData.alignH != AlignHJustify)) {
        d->alignmentData.alignH = AlignLeft;
    }
    d->alignmentData.indent = indent;
    d->dirty = true;
}

/*!
 * Return whether the cell is shrink to fit.
 */
bool Format::shrinkToFit() const
{
    return d->alignmentData.shinkToFit;
}

/*!
 * Turn on/off shrink to fit.
 */
void Format::setShrinkToFit(bool shink)
{
    if (shink && d->alignmentData.wrap)
        d->alignmentData.wrap = false;
    if (shink && (d->alignmentData.alignH == AlignHFill
                  || d->alignmentData.alignH == AlignHJustify
                  || d->alignmentData.alignH == AlignHDistributed)) {
        d->alignmentData.alignH = AlignLeft;
    }

    d->alignmentData.shinkToFit = shink;
    d->dirty = true;
}

/*!
 * \internal
 */
bool Format::alignmentChanged() const
{
    return d->alignmentData.alignH != AlignHGeneral
            || d->alignmentData.alignV != AlignBottom
            || d->alignmentData.indent != 0
            || d->alignmentData.wrap
            || d->alignmentData.rotation != 0
            || d->alignmentData.shinkToFit;
}

QString Format::horizontalAlignmentString() const
{
    QString alignH;
    switch (d->alignmentData.alignH) {
    case Format::AlignLeft:
        alignH = QStringLiteral("left");
        break;
    case Format::AlignHCenter:
        alignH = QStringLiteral("center");
        break;
    case Format::AlignRight:
        alignH = QStringLiteral("right");
        break;
    case Format::AlignHFill:
        alignH = QStringLiteral("fill");
        break;
    case Format::AlignHJustify:
        alignH = QStringLiteral("justify");
        break;
    case Format::AlignHMerge:
        alignH = QStringLiteral("centerContinuous");
        break;
    case Format::AlignHDistributed:
        alignH = QStringLiteral("distributed");
        break;
    default:
        break;
    }
    return alignH;
}

QString Format::verticalAlignmentString() const
{
    QString align;
    switch (d->alignmentData.alignV) {
    case AlignTop:
        align = QStringLiteral("top");
        break;
    case AlignVCenter:
        align = QStringLiteral("center");
        break;
    case AlignVJustify:
        align = QStringLiteral("justify");
        break;
    case AlignVDistributed:
        align = QStringLiteral("distributed");
        break;
    default:
        break;
    }
    return align;
}

/*!
 * Set the border style.
 */
void Format::setBorderStyle(BorderStyle style)
{
    setLeftBorderStyle(style);
    setRightBorderStyle(style);
    setBottomBorderStyle(style);
    setTopBorderStyle(style);
}

/*!
 * Set the border color.
 */
void Format::setBorderColor(const QColor &color)
{
    setLeftBorderColor(color);
    setRightBorderColor(color);
    setTopBorderColor(color);
    setBottomBorderColor(color);
}

/*!
 * Return the left border style
 */
Format::BorderStyle Format::leftBorderStyle() const
{
    return d->borderData.left;
}

/*!
 * Set the left border style
 */
void Format::setLeftBorderStyle(BorderStyle style)
{
    d->borderData.left = style;
    d->borderData._dirty = true;
}

/*!
 * Return the left border color
 */
QColor Format::leftBorderColor() const
{
    return d->borderData.leftColor;
}

void Format::setLeftBorderColor(const QColor &color)
{
    d->borderData.leftColor = color;
    d->borderData._dirty = true;
}

Format::BorderStyle Format::rightBorderStyle() const
{
    return d->borderData.right;
}

void Format::setRightBorderStyle(BorderStyle style)
{
    d->borderData.right = style;
    d->borderData._dirty = true;
}

QColor Format::rightBorderColor() const
{
    return d->borderData.rightColor;
}

void Format::setRightBorderColor(const QColor &color)
{
    d->borderData.rightColor = color;
    d->borderData._dirty = true;
}

Format::BorderStyle Format::topBorderStyle() const
{
    return d->borderData.top;
}

void Format::setTopBorderStyle(BorderStyle style)
{
    d->borderData.top = style;
    d->borderData._dirty = true;
}

QColor Format::topBorderColor() const
{
    return d->borderData.topColor;
}

void Format::setTopBorderColor(const QColor &color)
{
    d->borderData.topColor = color;
    d->borderData._dirty = true;
}

Format::BorderStyle Format::bottomBorderStyle() const
{
    return d->borderData.bottom;
}

void Format::setBottomBorderStyle(BorderStyle style)
{
    d->borderData.bottom = style;
    d->borderData._dirty = true;
}

QColor Format::bottomBorderColor() const
{
    return d->borderData.bottomColor;
}

void Format::setBottomBorderColor(const QColor &color)
{
    d->borderData.bottomColor = color;
    d->borderData._dirty = true;
}

Format::BorderStyle Format::diagonalBorderStyle() const
{
    return d->borderData.diagonal;
}

void Format::setDiagonalBorderStyle(BorderStyle style)
{
    d->borderData.diagonal = style;
    d->borderData._dirty = true;
}

Format::DiagonalBorderType Format::diagonalBorderType() const
{
    return d->borderData.diagonalType;
}

void Format::setDiagonalBorderType(DiagonalBorderType style)
{
    d->borderData.diagonalType = style;
    d->borderData._dirty = true;
}

QColor Format::diagonalBorderColor() const
{
    return d->borderData.diagonalColor;
}

void Format::setDiagonalBorderColor(const QColor &color)
{
    d->borderData.diagonalColor = color;
    d->borderData._dirty = true;
}

bool Format::borderIndexValid() const
{
    return d->borderData.indexValid();
}

int Format::borderIndex() const
{
    return d->borderData.index();
}

void Format::setBorderIndex(int index)
{
    d->borderData.setIndex(index);
}

/* Internal
 */
QByteArray Format::borderKey() const
{
    if (d->borderData._dirty)
        d->dirty = true; //Make sure formatKey() will be re-generated.

    return d->borderData.key();
}

Format::FillPattern Format::fillPattern() const
{
    return d->fillData.pattern;
}

void Format::setFillPattern(FillPattern pattern)
{
    d->fillData.pattern = pattern;
    d->fillData._dirty = true;
}

QColor Format::patternForegroundColor() const
{
    return d->fillData.fgColor;
}

void Format::setPatternForegroundColor(const QColor &color)
{
    if (color.isValid() && d->fillData.pattern == PatternNone)
        d->fillData.pattern = PatternSolid;
    d->fillData.fgColor = color;
    d->fillData._dirty = true;
}

QColor Format::patternBackgroundColor() const
{
    return d->fillData.bgColor;
}

void Format::setPatternBackgroundColor(const QColor &color)
{
    if (color.isValid() && d->fillData.pattern == PatternNone)
        d->fillData.pattern = PatternSolid;
    d->fillData.bgColor = color;
    d->fillData._dirty = true;
}

bool Format::fillIndexValid() const
{
    return d->fillData.indexValid();
}

int Format::fillIndex() const
{
    return d->fillData.index();
}

void Format::setFillIndex(int index)
{
    d->fillData.setIndex(index);
}

/* Internal
 */
QByteArray Format::fillKey() const
{
    if (d->fillData._dirty)
        d->dirty = true; //Make sure formatKey() will be re-generated.

    return d->fillData.key();
}

bool Format::hidden() const
{
    return d->protectionData.hidden;
}

void Format::setHidden(bool hidden)
{
    d->protectionData.hidden = hidden;
    d->dirty = true;
}

bool Format::locked() const
{
    return d->protectionData.locked;
}

void Format::setLocked(bool locked)
{
    d->protectionData.locked = locked;
    d->dirty = true;
}

QByteArray Format::formatKey() const
{
    if (d->dirty || d->fontData._dirty || d->borderData._dirty || d->fillData._dirty) {
        QByteArray key;
        QDataStream stream(&key, QIODevice::WriteOnly);
        stream<<fontKey()<<borderKey()<<fillKey()
             <<d->numberData.formatIndex
            <<d->alignmentData.alignH<<d->alignmentData.alignV<<d->alignmentData.indent
           <<d->alignmentData.rotation<<d->alignmentData.shinkToFit<<d->alignmentData.wrap
          <<d->protectionData.hidden<<d->protectionData.locked;
        d->formatKey = key;
        d->dirty = false;
        d->xf_indexValid = false;
        d->dxf_indexValid = false;
    }

    return d->formatKey;
}

void Format::setXfIndex(int index)
{
    d->xf_index = index;
    d->xf_indexValid = true;
}

int Format::xfIndex() const
{
    return d->xf_index;
}

bool Format::xfIndexValid() const
{
    return !d->dirty && d->xf_indexValid;
}

void Format::setDxfIndex(int index)
{
    d->dxf_index = index;
    d->dxf_indexValid = true;
}

int Format::dxfIndex() const
{
    return d->dxf_index;
}

bool Format::dxfIndexValid() const
{
    return !d->dirty && d->dxf_indexValid;
}

bool Format::operator ==(const Format &format) const
{
    return this->formatKey() == format.formatKey();
}

bool Format::operator !=(const Format &format) const
{
    return this->formatKey() != format.formatKey();
}

bool Format::isDxfFormat() const
{
    return d->is_dxf_fomat;
}

int Format::theme() const
{
    return d->theme;
}

QT_END_NAMESPACE_XLSX
