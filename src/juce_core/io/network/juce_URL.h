/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_URL_JUCEHEADER__
#define __JUCE_URL_JUCEHEADER__

#include "../../text/juce_StringPairArray.h"
#include "../../text/juce_XmlElement.h"
#include "../juce_InputStream.h"


//==============================================================================
/**
    Represents a URL and has a bunch of useful functions to manipulate it.

    This class can be used to launch URLs in browsers, and also to create
    InputStreams that can read from remote http or ftp sources.
*/
class JUCE_API  URL
{
public:
    //==============================================================================
    /** Creates an empty URL. */
    URL() throw();

    /** Creates a URL from a string. */
    URL (const String& url);

    /** Creates a copy of another URL. */
    URL (const URL& other);

    /** Destructor. */
    ~URL() throw();

    /** Copies this URL from another one. */
    const URL& operator= (const URL& other);

    //==============================================================================
    /** Returns a string version of the URL.

        If includeGetParameters is true and any parameters have been set with the
        withParameter() method, then the string will have these appended on the
        end and url-encoded.
    */
    const String toString (const bool includeGetParameters) const;

    /** True if it seems to be valid. */
    bool isWellFormed() const;

    /** Returns just the domain part of the URL.

        E.g. for "http://www.xyz.com/foobar", this will return "www.xyz.com".
    */
    const String getDomain() const;

    /** Returns the path part of the URL.

        E.g. for "http://www.xyz.com/foo/bar?x=1", this will return "foo/bar".
    */
    const String getSubPath() const;
    
    /** Returns the scheme of the URL. 

        E.g. for "http://www.xyz.com/foobar", this will return "http". (It won't 
        include the colon).
    */
    const String getScheme() const;
    
    /** Returns a new version of this URL that uses a different sub-path.
     
        E.g. if the URL is "http://www.xyz.com/foo?x=1" and you call this with
        "bar", it'll return "http://www.xyz.com/bar?x=1".
    */
    const URL withNewSubPath (const String& newPath) const;

    //==============================================================================
    /** Returns a copy of this URL, with a GET parameter added to the end.

        Any control characters in the value will be encoded.

        e.g. calling "withParameter ("amount", "some fish") for the url "www.fish.com"
        would produce a new url whose toString(true) method would return
        "www.fish.com?amount=some+fish".
    */
    const URL withParameter (const String& parameterName,
                             const String& parameterValue) const;

    /** Returns a copy of this URl, with a file-upload type parameter added to it.

        When performing a POST where one of your parameters is a binary file, this
        lets you specify the file.

        Note that the filename is stored, but the file itself won't actually be read
        until this URL is later used to create a network input stream.
    */
    const URL withFileToUpload (const String& parameterName,
                                const File& fileToUpload,
                                const String& mimeType) const;

    /** Returns a set of all the parameters encoded into the url.

        E.g. for the url "www.fish.com?type=haddock&amount=some+fish", this array would
        contain two pairs: "type" => "haddock" and "amount" => "some fish".

        The values returned will have been cleaned up to remove any escape characters.

        @see getNamedParameter, withParameter
    */
    const StringPairArray& getParameters() const throw();

    /** Returns the set of files that should be uploaded as part of a POST operation.

        This is the set of files that were added to the URL with the withFileToUpload()
        method.
    */
    const StringPairArray& getFilesToUpload() const throw();

    /** Returns the set of mime types associated with each of the upload files.
    */
    const StringPairArray& getMimeTypesOfUploadFiles() const throw();

    /** Returns a copy of this URL, with a block of data to send as the POST data.

        If you're setting the POST data, be careful not to have any parameters set
        as well, otherwise it'll all get thrown in together, and might not have the
        desired effect.

        If the URL already contains some POST data, this will replace it, rather
        than being appended to it.

        This data will only be used if you specify a post operation when you call
        createInputStream().
    */
    const URL withPOSTData (const String& postData) const;

    /** Returns the data that was set using withPOSTData().
    */
    const String getPostData() const throw()                        { return postData; }

    //==============================================================================
    /** Tries to launch the system's default browser to open the URL.

        Returns true if this seems to have worked.
    */
    bool launchInDefaultBrowser() const;

    //==============================================================================
    /** Takes a guess as to whether a string might be a valid website address.

        This isn't foolproof!
    */
    static bool isProbablyAWebsiteURL (const String& possibleURL);

    /** Takes a guess as to whether a string might be a valid email address.

        This isn't foolproof!
    */
    static bool isProbablyAnEmailAddress (const String& possibleEmailAddress);

    //==============================================================================
    /** This callback function can be used by the createInputStream() method.

        It allows your app to receive progress updates during a lengthy POST operation. If you
        want to continue the operation, this should return true, or false to abort.
    */
    typedef bool (OpenStreamProgressCallback) (void* context, int bytesSent, int totalBytes);

    /** Attempts to open a stream that can read from this URL.

        @param usePostCommand   if true, it will try to do use a http 'POST' to pass
                                the paramters, otherwise it'll encode them into the
                                URL and do a 'GET'.
        @param progressCallback if this is non-zero, it lets you supply a callback function
                                to keep track of the operation's progress. This can be useful
                                for lengthy POST operations, so that you can provide user feedback.
        @param progressCallbackContext  if a callback is specified, this value will be passed to
                                the function
        @param extraHeaders     if not empty, this string is appended onto the headers that
                                are used for the request. It must therefore be a valid set of HTML
                                header directives, separated by newlines.
        @param connectionTimeOutMs  if 0, this will use whatever default setting the OS chooses. If
                                a negative number, it will be infinite. Otherwise it specifies a
                                time in milliseconds.
    */
    InputStream* createInputStream (const bool usePostCommand,
                                    OpenStreamProgressCallback* const progressCallback = 0,
                                    void* const progressCallbackContext = 0,
                                    const String& extraHeaders = String::empty,
                                    const int connectionTimeOutMs = 0) const;


    //==============================================================================
    /** Tries to download the entire contents of this URL into a binary data block.

        If it succeeds, this will return true and append the data it read onto the end
        of the memory block.

        @param destData         the memory block to append the new data to
        @param usePostCommand   whether to use a POST command to get the data (uses
                                a GET command if this is false)
        @see readEntireTextStream, readEntireXmlStream
    */
    bool readEntireBinaryStream (MemoryBlock& destData,
                                 const bool usePostCommand = false) const;

    /** Tries to download the entire contents of this URL as a string.

        If it fails, this will return an empty string, otherwise it will return the
        contents of the downloaded file. If you need to distinguish between a read
        operation that fails and one that returns an empty string, you'll need to use
        a different method, such as readEntireBinaryStream().

        @param usePostCommand   whether to use a POST command to get the data (uses
                                a GET command if this is false)
        @see readEntireBinaryStream, readEntireXmlStream
    */
    const String readEntireTextStream (const bool usePostCommand = false) const;

    /** Tries to download the entire contents of this URL and parse it as XML.

        If it fails, or if the text that it reads can't be parsed as XML, this will
        return 0.

        When it returns a valid XmlElement object, the caller is responsibile for deleting
        this object when no longer needed.

        @param usePostCommand   whether to use a POST command to get the data (uses
                                a GET command if this is false)

        @see readEntireBinaryStream, readEntireTextStream
    */
    XmlElement* readEntireXmlStream (const bool usePostCommand = false) const;

    //==============================================================================
    /** Adds escape sequences to a string to encode any characters that aren't
        legal in a URL.

        E.g. any spaces will be replaced with "%20".

        This is the opposite of removeEscapeChars().

        If isParameter is true, it means that the string is going to be used
        as a parameter, so it also encodes '$' and ',' (which would otherwise
        be legal in a URL.

        @see removeEscapeChars
    */
    static const String addEscapeChars (const String& stringToAddEscapeCharsTo,
                                        const bool isParameter);

    /** Replaces any escape character sequences in a string with their original
        character codes.

        E.g. any instances of "%20" will be replaced by a space.

        This is the opposite of addEscapeChars().

        @see addEscapeChars
    */
    static const String removeEscapeChars (const String& stringToRemoveEscapeCharsFrom);


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    String url, postData;
    StringPairArray parameters, filesToUpload, mimeTypes;
};


#endif   // __JUCE_URL_JUCEHEADER__
