import QtQuick 2.8
import QtQuick.Layouts 1.0

import Tachidesk.Models 1.0

Item {
  property alias source: libraryModel.source
  SourcesLibraryModel {
    id: libraryModel
    nm: networkManager
  }

  LibraryBase {
    anchors.fill: parent
  }
}
