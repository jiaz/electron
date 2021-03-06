// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_ATOM_API_WEB_CONTENTS_H_
#define ATOM_BROWSER_API_ATOM_API_WEB_CONTENTS_H_

#include <string>
#include <vector>

#include "atom/browser/api/trackable_object.h"
#include "atom/browser/common_web_contents_delegate.h"
#include "content/public/common/favicon_url.h"
#include "content/public/browser/web_contents_observer.h"
#include "native_mate/handle.h"
#include "ui/gfx/image/image.h"

namespace brightray {
class InspectableWebContents;
}

namespace mate {
class Arguments;
class Dictionary;
}

namespace atom {

struct SetSizeParams;
class AtomBrowserContext;
class WebViewGuestDelegate;

namespace api {

class WebContents : public mate::TrackableObject<WebContents>,
                    public CommonWebContentsDelegate,
                    public content::WebContentsObserver {
 public:
  // For node.js callback function type: function(error, buffer)
  using PrintToPDFCallback =
      base::Callback<void(v8::Local<v8::Value>, v8::Local<v8::Value>)>;

  // Create from an existing WebContents.
  static mate::Handle<WebContents> CreateFrom(
      v8::Isolate* isolate, content::WebContents* web_contents);

  // Create a new WebContents.
  static mate::Handle<WebContents> Create(
      v8::Isolate* isolate, const mate::Dictionary& options);

  void Destroy();
  bool IsAlive() const;
  int GetID() const;
  bool Equal(const WebContents* web_contents) const;
  void LoadURL(const GURL& url, const mate::Dictionary& options);
  base::string16 GetTitle() const;
  bool IsLoading() const;
  bool IsWaitingForResponse() const;
  void Stop();
  void ReloadIgnoringCache();
  void GoBack();
  void GoForward();
  void GoToOffset(int offset);
  bool IsCrashed() const;
  void SetUserAgent(const std::string& user_agent);
  std::string GetUserAgent();
  void InsertCSS(const std::string& css);
  void ExecuteJavaScript(const base::string16& code);
  void OpenDevTools(mate::Arguments* args);
  void CloseDevTools();
  bool IsDevToolsOpened();
  void ToggleDevTools();
  void InspectElement(int x, int y);
  void InspectServiceWorker();
  v8::Local<v8::Value> Session(v8::Isolate* isolate);
  void HasServiceWorker(const base::Callback<void(bool)>&);
  void UnregisterServiceWorker(const base::Callback<void(bool)>&);
  void SetAudioMuted(bool muted);
  bool IsAudioMuted();
  void Print(mate::Arguments* args);

  // Print current page as PDF.
  void PrintToPDF(const base::DictionaryValue& setting,
                  const PrintToPDFCallback& callback);

  // DevTools workspace api.
  void AddWorkSpace(const base::FilePath& path);
  void RemoveWorkSpace(const base::FilePath& path);

  // Editing commands.
  void Undo();
  void Redo();
  void Cut();
  void Copy();
  void Paste();
  void PasteAndMatchStyle();
  void Delete();
  void SelectAll();
  void Unselect();
  void Replace(const base::string16& word);
  void ReplaceMisspelling(const base::string16& word);

  // Focus.
  void Focus();
  void TabTraverse(bool reverse);

  // Sending messages to browser.
  bool SendIPCMessage(const base::string16& channel,
                      const base::ListValue& args);

  // Methods for creating <webview>.
  void SetSize(const SetSizeParams& params);
  void SetAllowTransparency(bool allow);
  bool IsGuest() const;

 protected:
  explicit WebContents(content::WebContents* web_contents);
  explicit WebContents(const mate::Dictionary& options);
  ~WebContents();

  // mate::Wrappable:
  mate::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;
  bool IsDestroyed() const override;

  // content::WebContentsDelegate:
  bool AddMessageToConsole(content::WebContents* source,
                           int32 level,
                           const base::string16& message,
                           int32 line_no,
                           const base::string16& source_id) override;
  bool ShouldCreateWebContents(
      content::WebContents* web_contents,
      int route_id,
      int main_frame_route_id,
      WindowContainerType window_container_type,
      const base::string16& frame_name,
      const GURL& target_url,
      const std::string& partition_id,
      content::SessionStorageNamespace* session_storage_namespace) override;
  content::WebContents* OpenURLFromTab(
      content::WebContents* source,
      const content::OpenURLParams& params) override;
  void BeforeUnloadFired(content::WebContents* tab,
                         bool proceed,
                         bool* proceed_to_fire_unload) override;
  void MoveContents(content::WebContents* source,
                    const gfx::Rect& pos) override;
  void CloseContents(content::WebContents* source) override;
  void ActivateContents(content::WebContents* contents) override;
  bool IsPopupOrPanel(const content::WebContents* source) const override;
  void HandleKeyboardEvent(
      content::WebContents* source,
      const content::NativeWebKeyboardEvent& event) override;
  void EnterFullscreenModeForTab(content::WebContents* source,
                                 const GURL& origin) override;
  void ExitFullscreenModeForTab(content::WebContents* source) override;
  void RendererUnresponsive(content::WebContents* source) override;
  void RendererResponsive(content::WebContents* source) override;

  // content::WebContentsObserver:
  void BeforeUnloadFired(const base::TimeTicks& proceed_time) override;
  void RenderViewDeleted(content::RenderViewHost*) override;
  void RenderProcessGone(base::TerminationStatus status) override;
  void DocumentLoadedInFrame(
      content::RenderFrameHost* render_frame_host) override;
  void DidFinishLoad(content::RenderFrameHost* render_frame_host,
                     const GURL& validated_url) override;
  void DidFailLoad(content::RenderFrameHost* render_frame_host,
                   const GURL& validated_url,
                   int error_code,
                   const base::string16& error_description) override;
  void DidFailProvisionalLoad(content::RenderFrameHost* render_frame_host,
                              const GURL& validated_url,
                              int error_code,
                              const base::string16& error_description) override;
  void DidStartLoading() override;
  void DidStopLoading() override;
  void DidGetResourceResponseStart(
      const content::ResourceRequestDetails& details) override;
  void DidGetRedirectForResourceRequest(
      content::RenderFrameHost* render_frame_host,
      const content::ResourceRedirectDetails& details) override;
  void DidNavigateMainFrame(
      const content::LoadCommittedDetails& details,
      const content::FrameNavigateParams& params) override;
  bool OnMessageReceived(const IPC::Message& message) override;
  void WebContentsDestroyed() override;
  void NavigationEntryCommitted(
      const content::LoadCommittedDetails& load_details) override;
  void TitleWasSet(content::NavigationEntry* entry, bool explicit_set) override;
  void DidUpdateFaviconURL(
      const std::vector<content::FaviconURL>& urls) override;
  void PluginCrashed(const base::FilePath& plugin_path,
                     base::ProcessId plugin_pid) override;

 private:
  enum Type {
    BROWSER_WINDOW,  // Used by BrowserWindow.
    WEB_VIEW,  // Used by <webview>.
    REMOTE,  // Thin wrap around an existing WebContents.
  };

  AtomBrowserContext* GetBrowserContext() const;

  // Called when received a message from renderer.
  void OnRendererMessage(const base::string16& channel,
                         const base::ListValue& args);

  // Called when received a synchronous message from renderer.
  void OnRendererMessageSync(const base::string16& channel,
                             const base::ListValue& args,
                             IPC::Message* message);

  v8::Global<v8::Value> session_;

  scoped_ptr<WebViewGuestDelegate> guest_delegate_;

  // The type of current WebContents.
  Type type_;

  DISALLOW_COPY_AND_ASSIGN(WebContents);
};

}  // namespace api

}  // namespace atom

#endif  // ATOM_BROWSER_API_ATOM_API_WEB_CONTENTS_H_
