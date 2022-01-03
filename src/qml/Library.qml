import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

import Tachidesk.Models
import SortFilterProxyModel

Item {
  id: libraryBase
  SortFilterProxyModel {
    id: libraryModel
    sourceModel: library
    sorters: [
      RoleSorter {
        roleName: "title"
        sortOrder: Qt.AscendingOrder
      }
    ]
  }

  LibraryModel {
    id: library
    nm: networkManager
  }


  LibraryBase {
    anchors.fill: parent
  }
}
