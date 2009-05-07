//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2008 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "TextEngine.h"

#include "../base/Logger.h"
#include "../base/OSHelper.h"
#include "../base/Exception.h"
#include "../base/FileHelper.h"
#include "../base/StringHelper.h"

#include <pango/pango.h>
#include <pango/pangoft2.h>
#include <fontconfig/fontconfig.h>

#include <assert.h>
#include <algorithm>

namespace avg {

using namespace std;

static vector<string> s_fontDirs;
static bool s_bTextEngineLoaded = false;

static void
text_subst_func(FcPattern *pattern, gpointer data)
{
//  GimpText *text = GIMP_TEXT (data);

  FcPatternAddBool(pattern, FC_HINTING, true);
  FcPatternAddInteger(pattern, FC_HINT_STYLE, FC_HINT_MEDIUM);
  FcPatternAddInteger(pattern, FC_RGBA, FC_RGBA_NONE);
  FcPatternAddBool(pattern, FC_ANTIALIAS, true);
}

TextEngine& TextEngine::get() 
{
    static TextEngine s_Instance;
    return s_Instance;
}


TextEngine::TextEngine()
{
    s_bTextEngineLoaded = true;
    pango_ft2_get_context(72, 72);

    PangoFT2FontMap *pFontMap;
    pFontMap = PANGO_FT2_FONT_MAP (pango_ft2_font_map_new());
    pango_ft2_font_map_set_resolution (pFontMap, 72, 72);
    pango_ft2_font_map_set_default_substitute (pFontMap, text_subst_func, 0, 0);
    m_pPangoContext = pango_ft2_font_map_create_context (pFontMap);
    g_object_unref (pFontMap);

    pango_context_set_language(m_pPangoContext,
            pango_language_from_string ("en_US"));
    pango_context_set_base_dir(m_pPangoContext, PANGO_DIRECTION_LTR);

    initFonts();

    string sOldLang = "";
    getEnv("LC_CTYPE", sOldLang);
    setEnv("LC_CTYPE", "en-us");
    pango_font_map_list_families(PANGO_FONT_MAP(pFontMap), &m_ppFontFamilies, 
            &m_NumFontFamilies);
    setEnv("LC_CTYPE", sOldLang);
    for (int i=0; i<m_NumFontFamilies; ++i) {
        m_sFonts.push_back(pango_font_family_get_name(m_ppFontFamilies[i]));
    }
    sort(m_sFonts.begin(), m_sFonts.end());
}

TextEngine::~TextEngine()
{
}

void TextEngine::addFontDir(const std::string& sDir)
{
    if (s_bTextEngineLoaded) {
        throw(Exception(AVG_ERR_UNSUPPORTED,
                "addFontDir() should be called before Player.play()"));
    }
    s_fontDirs.push_back(sDir);
}

PangoContext * TextEngine::getPangoContext()
{
    return m_pPangoContext;
}

const vector<string>& TextEngine::getFontFamilies()
{
    return m_sFonts;
}

const vector<string>& TextEngine::getFontVariants(const string& sFontName)
{
    PangoFontFamily * pCurFamily = getFontFamily(sFontName);
    PangoFontFace ** ppFaces;
    int numFaces;
    pango_font_family_list_faces (pCurFamily, &ppFaces, &numFaces);
    static vector<string> sVariants;
    for (int i=0; i<numFaces; ++i) {
        sVariants.push_back(pango_font_face_get_face_name(ppFaces[i]));
    }
    g_free(ppFaces);
    return sVariants;
}

PangoFontDescription * TextEngine::getFontDescription(const string& sFamily, 
        const string& sVariant)
{
    PangoFontDescription* pDescription;
    FontDescriptionCache::iterator it;
    it = m_FontDescriptionCache.find(pair<string, string>(sFamily, sVariant));
    if (it == m_FontDescriptionCache.end()) {
        PangoFontFamily * pFamily;
        bool bFamilyFound = true;
        try {
            pFamily = getFontFamily(sFamily);
        } catch (Exception&) {
            if (m_sFontsNotFound.find(sFamily) == m_sFontsNotFound.end()) {
                AVG_TRACE(Logger::WARNING, "Could not find font face " << sFamily << 
                        ". Using sans instead.");
                m_sFontsNotFound.insert(sFamily);
            }
            bFamilyFound = false;
            pFamily = getFontFamily("sans");
        }
        PangoFontFace ** ppFaces;
        int numFaces;
        pango_font_family_list_faces(pFamily, &ppFaces, &numFaces);
        PangoFontFace * pFace = 0;
        if (sVariant == "") {
            pFace = ppFaces[0];
        } else {
            for (int i=0; i<numFaces; ++i) {
                if (equalIgnoreCase(pango_font_face_get_face_name(ppFaces[i]), sVariant)) {
                    pFace = ppFaces[i];
                }
            }
        }
        if (!pFace) {
            pFace = ppFaces[0];
            if (bFamilyFound) {
                pair<string, string> variant(sFamily, sVariant);
                if (m_VariantsNotFound.find(variant) == m_VariantsNotFound.end()) {
                    m_VariantsNotFound.insert(variant);
                    AVG_TRACE(Logger::WARNING, "Could not find font variant " 
                            << sFamily << ":" << sVariant << ". Using " <<
                            pango_font_face_get_face_name(pFace) << " instead.");
                }
            }
        }
        g_free(ppFaces);
        pDescription = pango_font_face_describe(pFace);
        m_FontDescriptionCache[pair<string, string>(sFamily, sVariant)] =
                pDescription;
    } else {
        pDescription = it->second;
    }
    return pango_font_description_copy(pDescription);
}

void GLibLogFunc(const gchar *log_domain, GLogLevelFlags log_level, 
        const gchar *message, gpointer unused_data)
{
    string s = "Pango ";
    if (log_level & G_LOG_LEVEL_ERROR) {
        s += "error: ";
    } else if (log_level & G_LOG_LEVEL_CRITICAL) {
        s += string("critical: ")+message;
        AVG_TRACE(Logger::ERROR, s);
        assert(false);
    } else if (log_level & G_LOG_LEVEL_WARNING) {
        s += "warning: ";
    } else if (log_level & G_LOG_LEVEL_MESSAGE) {
        s += "message: ";
    } else if (log_level & G_LOG_LEVEL_INFO) {
        s += "info: ";
    } else if (log_level & G_LOG_LEVEL_DEBUG) {
        s += "debug: ";
    }
    s += message;
    AVG_TRACE(Logger::WARNING, s);
}

void TextEngine::initFonts()
{
    g_type_init();
    std::string sFontConfPath = "/etc/fonts/fonts.conf"; 
    if (!fileExists(sFontConfPath)) {
        sFontConfPath = getAvgLibPath()+"etc/fonts/fonts.conf";
    }
    FcConfig * pConfig = FcConfigCreate();
    int Ok = (int)FcConfigParseAndLoad(pConfig, 
            (const FcChar8 *)(sFontConfPath.c_str()), true);
    checkFontError(Ok, string("Font error: could not load config file ")+sFontConfPath);
    Ok = (int)FcConfigBuildFonts(pConfig);
    checkFontError(Ok, string("Font error: FcConfigBuildFonts failed."));
    Ok = (int)FcConfigSetCurrent(pConfig);
    checkFontError(Ok, string("Font error: FcConfigSetCurrent failed."));
    s_fontDirs.push_back("fonts/");
    for(std::vector<std::string>::const_iterator it = s_fontDirs.begin();
            it != s_fontDirs.end(); ++it) {
        Ok = (int)FcConfigAppFontAddDir(pConfig, (const FcChar8 *)it->c_str());
        checkFontError(Ok, string("Font error: FcConfigAppFontAddDir("
                    + *it + ") failed."));
    }
    /*
       FcStrList * pCacheDirs = FcConfigGetCacheDirs(pConfig);
       FcChar8 * pDir;
       do {
       pDir = FcStrListNext(pCacheDirs);
       if (pDir) {
       cerr << pDir << endl;
       }
       } while (pDir);
     */
    g_log_set_default_handler(GLibLogFunc, 0);
}

PangoFontFamily * TextEngine::getFontFamily(const string& sFamily)
{
    PangoFontFamily * pFamily = 0;
    assert(m_NumFontFamilies != 0);
    for (int i=0; i<m_NumFontFamilies; ++i) {
        if (equalIgnoreCase(pango_font_family_get_name(m_ppFontFamilies[i]), sFamily)) {
            pFamily = m_ppFontFamilies[i];
        }
    }
    if (!pFamily) {
        throw(Exception(AVG_ERR_INVALID_ARGS, 
                "getFontFamily: Font family "+sFamily+" not found."));
    }
    return pFamily;
}

void TextEngine::checkFontError(int Ok, const string& sMsg)
{
    if (Ok == 0) {
        throw Exception(AVG_ERR_FONT_INIT_FAILED, sMsg);
    }
}

}