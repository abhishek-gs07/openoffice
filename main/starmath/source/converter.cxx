/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/

#include "precompiled_starmath.hxx"

#include "converter.hxx"

#include <vector>

namespace
{
    enum TokenType
    {
        TOKEN_TEXT,
        TOKEN_COMMAND,
        TOKEN_LEFT_GROUP,
        TOKEN_RIGHT_GROUP,
        TOKEN_SUPERSCRIPT,
        TOKEN_SUBSCRIPT
    };

    struct Token
    {
        TokenType eType;
        String aText;

        Token(TokenType eTokenType, const String &rText)
            : eType(eTokenType), aText(rText)
        {
        }
    };

    sal_Bool IsAsciiLetter(sal_Unicode cChar)
    {
        return (('a' <= cChar) && (cChar <= 'z'))
            || (('A' <= cChar) && (cChar <= 'Z'));
    }

    class Tokenizer
    {
        const String &mrInput;
        xub_StrLen mnPosition;
        std::vector<Token> maTokens;

        void AddText(xub_StrLen nStart, xub_StrLen nLength)
        {
            if (nLength)
                maTokens.push_back(Token(TOKEN_TEXT,
                                         mrInput.Copy(nStart, nLength)));
        }

    public:
        explicit Tokenizer(const String &rInput)
            : mrInput(rInput), mnPosition(0)
        {
            while (mnPosition < mrInput.Len())
            {
                const xub_StrLen nStart = mnPosition;
                const sal_Unicode cChar = mrInput.GetChar(mnPosition);

                if (cChar == '{' || cChar == '}' || cChar == '^' || cChar == '_')
                {
                    ++mnPosition;
                    TokenType eType = TOKEN_TEXT;
                    if (cChar == '{')
                        eType = TOKEN_LEFT_GROUP;
                    else if (cChar == '}')
                        eType = TOKEN_RIGHT_GROUP;
                    else if (cChar == '^')
                        eType = TOKEN_SUPERSCRIPT;
                    else
                        eType = TOKEN_SUBSCRIPT;
                    maTokens.push_back(Token(eType, mrInput.Copy(nStart, 1)));
                }
                else if (cChar == '\\')
                {
                    ++mnPosition;
                    const xub_StrLen nNameStart = mnPosition;
                    while (mnPosition < mrInput.Len()
                           && IsAsciiLetter(mrInput.GetChar(mnPosition)))
                        ++mnPosition;

                    if (mnPosition == nNameStart)
                    {
                        if (mnPosition < mrInput.Len())
                            ++mnPosition;
                        if (mnPosition == nNameStart + 1)
                            maTokens.push_back(Token(TOKEN_COMMAND,
                                mrInput.Copy(nNameStart, 1)));
                        else
                            AddText(nStart, mnPosition - nStart);
                    }
                    else
                    {
                        maTokens.push_back(Token(TOKEN_COMMAND,
                            mrInput.Copy(nNameStart, mnPosition - nNameStart)));
                    }
                }
                else
                {
                    ++mnPosition;
                    while (mnPosition < mrInput.Len())
                    {
                        const sal_Unicode cNext = mrInput.GetChar(mnPosition);
                        if (cNext == '{' || cNext == '}' || cNext == '^'
                            || cNext == '_' || cNext == '\\')
                            break;
                        ++mnPosition;
                    }
                    AddText(nStart, mnPosition - nStart);
                }
            }
        }

        const std::vector<Token> &GetTokens() const
        {
            return maTokens;
        }
    };

    class Parser
    {
        const std::vector<Token> &mrTokens;
        const SmConverter::CommandTable &mrCommands;
        size_t mnPosition;

        size_t ArgumentEnd(size_t nPosition) const
        {
            if (nPosition >= mrTokens.size()
                || mrTokens[nPosition].eType == TOKEN_RIGHT_GROUP)
                return nPosition;

            if (mrTokens[nPosition].eType != TOKEN_LEFT_GROUP)
                return nPosition + 1;

            size_t nDepth = 1;
            ++nPosition;
            while (nPosition < mrTokens.size() && nDepth)
            {
                if (mrTokens[nPosition].eType == TOKEN_LEFT_GROUP)
                    ++nDepth;
                else if (mrTokens[nPosition].eType == TOKEN_RIGHT_GROUP)
                    --nDepth;
                ++nPosition;
            }
            return nPosition;
        }

        size_t SkipWhitespace(size_t nPosition) const
        {
            while (nPosition < mrTokens.size()
                   && mrTokens[nPosition].eType == TOKEN_TEXT)
            {
                const String &rText = mrTokens[nPosition].aText;
                xub_StrLen nChar = 0;
                while (nChar < rText.Len())
                {
                    const sal_Unicode cChar = rText.GetChar(nChar);
                    if (cChar != ' ' && cChar != '\t' && cChar != '\n'
                        && cChar != '\r')
                        break;
                    ++nChar;
                }
                if (nChar != rText.Len())
                    break;
                ++nPosition;
            }
            return nPosition;
        }

        String ParseCommand(const String &rName)
        {
            if (rName.EqualsAscii("frac") || rName.EqualsAscii("dfrac")
                || rName.EqualsAscii("tfrac"))
            {
                const size_t nFirstStart = SkipWhitespace(mnPosition);
                const size_t nFirstEnd = ArgumentEnd(nFirstStart);
                const size_t nSecondStart = SkipWhitespace(nFirstEnd);
                const size_t nSecondEnd = ArgumentEnd(nSecondStart);
                if (nFirstEnd > nFirstStart && nSecondEnd > nSecondStart)
                {
                    mnPosition = nFirstStart;
                    const String aNumerator(ParseAtom());
                    mnPosition = nSecondStart;
                    const String aDenominator(ParseAtom());
                    String aResult(aNumerator);
                    aResult.AppendAscii(" over ");
                    aResult.Append(aDenominator);
                    return aResult;
                }
            }

            SmConverter::CommandTable::const_iterator aCommand =
                mrCommands.find(rName);
            if (aCommand != mrCommands.end())
                return aCommand->second;

            String aResult("\\");
            aResult.Append(rName);
            return aResult;
        }

        String ParseAtom()
        {
            if (mnPosition >= mrTokens.size())
                return String();

            const Token &rToken = mrTokens[mnPosition++];
            if (rToken.eType == TOKEN_LEFT_GROUP)
            {
                String aResult("{");
                aResult.Append(ParseSequence(sal_True));
                aResult.Append('}');
                return aResult;
            }
            else if (rToken.eType == TOKEN_COMMAND)
                return ParseCommand(rToken.aText);
            return rToken.aText;
        }

        String ParseSequence(sal_Bool bInGroup)
        {
            String aResult;
            while (mnPosition < mrTokens.size())
            {
                const Token &rToken = mrTokens[mnPosition];
                if (rToken.eType == TOKEN_RIGHT_GROUP)
                {
                    ++mnPosition;
                    if (bInGroup)
                        return aResult;
                    aResult.Append(rToken.aText);
                }
                else if (rToken.eType == TOKEN_LEFT_GROUP)
                    aResult.Append(ParseAtom());
                else if (rToken.eType == TOKEN_SUPERSCRIPT
                         || rToken.eType == TOKEN_SUBSCRIPT)
                {
                    aResult.Append(rToken.aText);
                    ++mnPosition;
                    if (mnPosition < mrTokens.size()
                        && mrTokens[mnPosition].eType != TOKEN_RIGHT_GROUP)
                        aResult.Append(ParseAtom());
                }
                else
                    aResult.Append(ParseAtom());
            }
            return aResult;
        }

    public:
        Parser(const std::vector<Token> &rTokens,
               const SmConverter::CommandTable &rCommands)
            : mrTokens(rTokens), mrCommands(rCommands), mnPosition(0)
        {
        }

        String Parse()
        {
            return ParseSequence(sal_False);
        }
    };

    void AddCommand(SmConverter::CommandTable &rCommands,
                    const sal_Char *pName, const sal_Char *pReplacement)
    {
        rCommands[String::CreateFromAscii(pName)] =
            String::CreateFromAscii(pReplacement);
    }
}

String SmConverter::Convert(const String &rInput, const CommandTable &rCommands)
{
    const Tokenizer aTokenizer(rInput);
    Parser aParser(aTokenizer.GetTokens(), rCommands);
    return aParser.Parse();
}

String SmConverter::ConvertLatex(const String &rInput)
{
    CommandTable aCommands;
    AddCommand(aCommands, "alpha", "alpha");
    AddCommand(aCommands, "beta", "beta");
    AddCommand(aCommands, "gamma", "gamma");
    AddCommand(aCommands, "delta", "delta");
    AddCommand(aCommands, "theta", "theta");
    AddCommand(aCommands, "lambda", "lambda");
    AddCommand(aCommands, "mu", "mu");
    AddCommand(aCommands, "pi", "pi");
    AddCommand(aCommands, "sigma", "sigma");
    AddCommand(aCommands, "phi", "phi");
    AddCommand(aCommands, "omega", "omega");
    AddCommand(aCommands, "infty", "infinity");
    AddCommand(aCommands, "sum", "sum");
    AddCommand(aCommands, "prod", "prod");
    AddCommand(aCommands, "int", "int");
    AddCommand(aCommands, "oint", " oint ");
    AddCommand(aCommands, "sqrt", "sqrt ");
    AddCommand(aCommands, "times", "times");
    AddCommand(aCommands, "cdot", "cdot");
    AddCommand(aCommands, "pm", "plusminus");
    AddCommand(aCommands, "mp", "minusplus");
    AddCommand(aCommands, "leq", "le");
    AddCommand(aCommands, "geq", "ge");
    AddCommand(aCommands, "neq", "neq");
    AddCommand(aCommands, "approx", "approx");
    AddCommand(aCommands, "sin", "sin ");
    AddCommand(aCommands, "cos", "cos ");
    AddCommand(aCommands, "tan", "tan ");
    AddCommand(aCommands, "log", "log ");
    AddCommand(aCommands, "ln", "ln ");
    AddCommand(aCommands, "lim", "lim ");
    AddCommand(aCommands, "left", "left ");
    AddCommand(aCommands, "right", "right ");
    AddCommand(aCommands, "vec", "widevec ");
    AddCommand(aCommands, "overline", "overline ");
    AddCommand(aCommands, "underline", "underline ");
    AddCommand(aCommands, ",", " ");
    AddCommand(aCommands, ";", " ");
    AddCommand(aCommands, "!", "");
    return Convert(rInput, aCommands);
}