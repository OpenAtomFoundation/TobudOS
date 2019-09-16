/*
 * Copyright (c) 2018 THL A29 Limited, a Tencent company. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
const AbstractModel = require("./iotmodelbase.js");

/**
 * DescribeProject返回参数结构体
 * @class
 */
class DescribeProjectResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 返回信息
     * @type {ProjectEntryEx || null}
     */
    this.Project = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Project) {
      let obj = new ProjectEntryEx();
      obj.deserialize(params.Project)
      this.Project = obj;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * ControlDeviceData返回参数结构体
 * @class
 */
class ControlDeviceDataResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 返回信息
     * @type {string || null}
     */
    this.Data = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.Data = 'Data' in params ? params.Data : null;
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * DescribeDeviceDataHistory请求参数结构体
 * @class
 */
class DescribeDeviceDataHistoryRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 区间开始时间
     * @type {number || null}
     */
    this.MinTime = null;

    /**
     * 区间结束时间
     * @type {number || null}
     */
    this.MaxTime = null;

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

    /**
     * 设备名称
     * @type {string || null}
     */
    this.DeviceName = null;

    /**
     * 属性字段名称
     * @type {string || null}
     */
    this.FieldName = null;

    /**
     * 返回条数
     * @type {number || null}
     */
    this.Limit = null;

    /**
     * 检索上下文
     * @type {string || null}
     */
    this.Context = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.MinTime = 'MinTime' in params ? params.MinTime : null;
    this.MaxTime = 'MaxTime' in params ? params.MaxTime : null;
    this.ProductId = 'ProductId' in params ? params.ProductId : null;
    this.DeviceName = 'DeviceName' in params ? params.DeviceName : null;
    this.FieldName = 'FieldName' in params ? params.FieldName : null;
    this.Limit = 'Limit' in params ? params.Limit : null;
    this.Context = 'Context' in params ? params.Context : null;

  }
}

/**
 * DescribeStudioProduct请求参数结构体
 * @class
 */
class DescribeStudioProductRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;

  }
}

/**
 * DeleteProject请求参数结构体
 * @class
 */
class DeleteProjectRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目ID
     * @type {number || null}
     */
    this.ProjectId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;

  }
}

/**
 * 设备历史数据结构
 * @class
 */
class DeviceDataHistoryItem extends AbstractModel {
  constructor() {
    super();

    /**
     * 时间点，毫秒时间戳
     * @type {string || null}
     */
    this.Time = null;

    /**
     * 字段取值
     * @type {string || null}
     */
    this.Value = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.Time = 'Time' in params ? params.Time : null;
    this.Value = 'Value' in params ? params.Value : null;

  }
}

/**
 * 产品详情
 * @class
 */
class ProductEntry extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

    /**
     * 产品名称
     * @type {string || null}
     */
    this.ProductName = null;

    /**
     * 产品分组模板ID
     * @type {number || null}
     */
    this.CategoryId = null;

    /**
     * 加密类型
     * @type {string || null}
     */
    this.EncryptionType = null;

    /**
     * 连接类型
     * @type {string || null}
     */
    this.NetType = null;

    /**
     * 数据协议
     * @type {number || null}
     */
    this.DataProtocol = null;

    /**
     * 产品描述
     * @type {string || null}
     */
    this.ProductDesc = null;

    /**
     * 状态
     * @type {string || null}
     */
    this.DevStatus = null;

    /**
     * 创建时间
     * @type {number || null}
     */
    this.CreateTime = null;

    /**
     * 更新时间
     * @type {number || null}
     */
    this.UpdateTime = null;

    /**
     * 区域
     * @type {string || null}
     */
    this.Region = null;

    /**
     * 产品类型
     * @type {number || null}
     */
    this.ProductType = null;

    /**
     * 项目ID
     * @type {string || null}
     */
    this.ProjectId = null;

    /**
     * 产品ModuleId
     * @type {number || null}
     */
    this.ModuleId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;
    this.ProductName = 'ProductName' in params ? params.ProductName : null;
    this.CategoryId = 'CategoryId' in params ? params.CategoryId : null;
    this.EncryptionType = 'EncryptionType' in params ? params.EncryptionType : null;
    this.NetType = 'NetType' in params ? params.NetType : null;
    this.DataProtocol = 'DataProtocol' in params ? params.DataProtocol : null;
    this.ProductDesc = 'ProductDesc' in params ? params.ProductDesc : null;
    this.DevStatus = 'DevStatus' in params ? params.DevStatus : null;
    this.CreateTime = 'CreateTime' in params ? params.CreateTime : null;
    this.UpdateTime = 'UpdateTime' in params ? params.UpdateTime : null;
    this.Region = 'Region' in params ? params.Region : null;
    this.ProductType = 'ProductType' in params ? params.ProductType : null;
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;
    this.ModuleId = 'ModuleId' in params ? params.ModuleId : null;

  }
}

/**
 * GetStudioProductList返回参数结构体
 * @class
 */
class GetStudioProductListResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品列表
     * @type {Array.<ProductEntry> || null}
     */
    this.Products = null;

    /**
     * 产品数量
     * @type {number || null}
     */
    this.Total = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Products) {
      this.Products = new Array();
      for (let z in params.Products) {
        let obj = new ProductEntry();
        obj.deserialize(params.Products[z]);
        this.Products.push(obj);
      }
    }
    this.Total = 'Total' in params ? params.Total : null;
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * DescribeModelDefinition返回参数结构体
 * @class
 */
class DescribeModelDefinitionResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品数据模板
     * @type {ProductModelDefinition || null}
     */
    this.Model = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Model) {
      let obj = new ProductModelDefinition();
      obj.deserialize(params.Model)
      this.Model = obj;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * 项目详情
 * @class
 */
class ProjectEntryEx extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目ID
     * @type {number || null}
     */
    this.ProjectId = null;

    /**
     * 项目名称
     * @type {string || null}
     */
    this.ProjectName = null;

    /**
     * 项目描述
     * @type {string || null}
     */
    this.ProjectDesc = null;

    /**
     * 项目创建时间，unix时间戳
     * @type {number || null}
     */
    this.CreateTime = null;

    /**
     * 项目更新时间，unix时间戳
     * @type {number || null}
     */
    this.UpdateTime = null;

    /**
     * 产品数量
     * @type {number || null}
     */
    this.ProductCount = null;

    /**
     * NativeApp数量
     * @type {number || null}
     */
    this.NativeAppCount = null;

    /**
     * WebApp数量
     * @type {number || null}
     */
    this.WebAppCount = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;
    this.ProjectName = 'ProjectName' in params ? params.ProjectName : null;
    this.ProjectDesc = 'ProjectDesc' in params ? params.ProjectDesc : null;
    this.CreateTime = 'CreateTime' in params ? params.CreateTime : null;
    this.UpdateTime = 'UpdateTime' in params ? params.UpdateTime : null;
    this.ProductCount = 'ProductCount' in params ? params.ProductCount : null;
    this.NativeAppCount = 'NativeAppCount' in params ? params.NativeAppCount : null;
    this.WebAppCount = 'WebAppCount' in params ? params.WebAppCount : null;

  }
}

/**
 * GetProjectList请求参数结构体
 * @class
 */
class GetProjectListRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 偏移量
     * @type {number || null}
     */
    this.Offset = null;

    /**
     * 个数限制
     * @type {number || null}
     */
    this.Limit = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.Offset = 'Offset' in params ? params.Offset : null;
    this.Limit = 'Limit' in params ? params.Limit : null;

  }
}

/**
 * DeleteStudioProduct返回参数结构体
 * @class
 */
class DeleteStudioProductResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * ModifyStudioProduct返回参数结构体
 * @class
 */
class ModifyStudioProductResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品描述
     * @type {ProductEntry || null}
     */
    this.Product = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Product) {
      let obj = new ProductEntry();
      obj.deserialize(params.Product)
      this.Product = obj;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * ControlDeviceData请求参数结构体
 * @class
 */
class ControlDeviceDataRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

    /**
     * 设备名称
     * @type {string || null}
     */
    this.DeviceName = null;

    /**
     * 属性数据
     * @type {string || null}
     */
    this.Data = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;
    this.DeviceName = 'DeviceName' in params ? params.DeviceName : null;
    this.Data = 'Data' in params ? params.Data : null;

  }
}

/**
 * SearchStudioProduct请求参数结构体
 * @class
 */
class SearchStudioProductRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目ID
     * @type {string || null}
     */
    this.ProjectId = null;

    /**
     * 产品名称
     * @type {string || null}
     */
    this.ProductName = null;

    /**
     * 列表Limit
     * @type {number || null}
     */
    this.Limit = null;

    /**
     * 列表Offset
     * @type {number || null}
     */
    this.Offset = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;
    this.ProductName = 'ProductName' in params ? params.ProductName : null;
    this.Limit = 'Limit' in params ? params.Limit : null;
    this.Offset = 'Offset' in params ? params.Offset : null;

  }
}

/**
 * ReleaseStudioProduct返回参数结构体
 * @class
 */
class ReleaseStudioProductResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * 项目详情
 * @class
 */
class ProjectEntry extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目ID
     * @type {number || null}
     */
    this.ProjectId = null;

    /**
     * 项目名称
     * @type {string || null}
     */
    this.ProjectName = null;

    /**
     * 项目描述
     * @type {string || null}
     */
    this.ProjectDesc = null;

    /**
     * 创建时间，unix时间戳
     * @type {number || null}
     */
    this.CreateTime = null;

    /**
     * 更新时间，unix时间戳
     * @type {number || null}
     */
    this.UpdateTime = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;
    this.ProjectName = 'ProjectName' in params ? params.ProjectName : null;
    this.ProjectDesc = 'ProjectDesc' in params ? params.ProjectDesc : null;
    this.CreateTime = 'CreateTime' in params ? params.CreateTime : null;
    this.UpdateTime = 'UpdateTime' in params ? params.UpdateTime : null;

  }
}

/**
 * ModifyProject返回参数结构体
 * @class
 */
class ModifyProjectResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目详情
     * @type {ProjectEntry || null}
     */
    this.Project = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Project) {
      let obj = new ProjectEntry();
      obj.deserialize(params.Project)
      this.Project = obj;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * ModifyModelDefinition请求参数结构体
 * @class
 */
class ModifyModelDefinitionRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

    /**
     * 数据模板定义
     * @type {string || null}
     */
    this.ModelSchema = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;
    this.ModelSchema = 'ModelSchema' in params ? params.ModelSchema : null;

  }
}

/**
 * CreateStudioProduct返回参数结构体
 * @class
 */
class CreateStudioProductResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品描述
     * @type {ProductEntry || null}
     */
    this.Product = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Product) {
      let obj = new ProductEntry();
      obj.deserialize(params.Product)
      this.Product = obj;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * DeleteStudioProduct请求参数结构体
 * @class
 */
class DeleteStudioProductRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;

  }
}

/**
 * ModifyProject请求参数结构体
 * @class
 */
class ModifyProjectRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目ID
     * @type {number || null}
     */
    this.ProjectId = null;

    /**
     * 项目名称
     * @type {string || null}
     */
    this.ProjectName = null;

    /**
     * 项目描述
     * @type {string || null}
     */
    this.ProjectDesc = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;
    this.ProjectName = 'ProjectName' in params ? params.ProjectName : null;
    this.ProjectDesc = 'ProjectDesc' in params ? params.ProjectDesc : null;

  }
}

/**
 * ModifyStudioProduct请求参数结构体
 * @class
 */
class ModifyStudioProductRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

    /**
     * 产品名称
     * @type {string || null}
     */
    this.ProductName = null;

    /**
     * 产品描述
     * @type {string || null}
     */
    this.ProductDesc = null;

    /**
     * 模型ID
     * @type {number || null}
     */
    this.ModuleId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;
    this.ProductName = 'ProductName' in params ? params.ProductName : null;
    this.ProductDesc = 'ProductDesc' in params ? params.ProductDesc : null;
    this.ModuleId = 'ModuleId' in params ? params.ModuleId : null;

  }
}

/**
 * DescribeDeviceDataHistory返回参数结构体
 * @class
 */
class DescribeDeviceDataHistoryResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 属性字段名称
注意：此字段可能返回 null，表示取不到有效值。
     * @type {string || null}
     */
    this.FieldName = null;

    /**
     * 数据是否已全部返回，true 表示数据全部返回，false 表示还有数据待返回，可将 Context 作为入参，继续查询返回结果。
注意：此字段可能返回 null，表示取不到有效值。
     * @type {boolean || null}
     */
    this.Listover = null;

    /**
     * 检索上下文，当 ListOver 为false时，可以用此上下文，继续读取后续数据
注意：此字段可能返回 null，表示取不到有效值。
     * @type {string || null}
     */
    this.Context = null;

    /**
     * 历史数据结果数组，返回对应时间点及取值。
注意：此字段可能返回 null，表示取不到有效值。
     * @type {Array.<DeviceDataHistoryItem> || null}
     */
    this.Results = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.FieldName = 'FieldName' in params ? params.FieldName : null;
    this.Listover = 'Listover' in params ? params.Listover : null;
    this.Context = 'Context' in params ? params.Context : null;

    if (params.Results) {
      this.Results = new Array();
      for (let z in params.Results) {
        let obj = new DeviceDataHistoryItem();
        obj.deserialize(params.Results[z]);
        this.Results.push(obj);
      }
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * CreateStudioProduct请求参数结构体
 * @class
 */
class CreateStudioProductRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品名称
     * @type {string || null}
     */
    this.ProductName = null;

    /**
     * 产品分组模板ID
     * @type {number || null}
     */
    this.CategoryId = null;

    /**
     * 产品类型
     * @type {number || null}
     */
    this.ProductType = null;

    /**
     * 加密类型
     * @type {string || null}
     */
    this.EncryptionType = null;

    /**
     * 连接类型
     * @type {string || null}
     */
    this.NetType = null;

    /**
     * 数据协议
     * @type {number || null}
     */
    this.DataProtocol = null;

    /**
     * 产品描述
     * @type {string || null}
     */
    this.ProductDesc = null;

    /**
     * 产品的项目ID
     * @type {string || null}
     */
    this.ProjectId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductName = 'ProductName' in params ? params.ProductName : null;
    this.CategoryId = 'CategoryId' in params ? params.CategoryId : null;
    this.ProductType = 'ProductType' in params ? params.ProductType : null;
    this.EncryptionType = 'EncryptionType' in params ? params.EncryptionType : null;
    this.NetType = 'NetType' in params ? params.NetType : null;
    this.DataProtocol = 'DataProtocol' in params ? params.DataProtocol : null;
    this.ProductDesc = 'ProductDesc' in params ? params.ProductDesc : null;
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;

  }
}

/**
 * 产品模型定义
 * @class
 */
class ProductModelDefinition extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

    /**
     * 模型定义
     * @type {string || null}
     */
    this.ModelDefine = null;

    /**
     * 更新时间，秒级时间戳
     * @type {number || null}
     */
    this.UpdateTime = null;

    /**
     * 创建时间，秒级时间戳
     * @type {number || null}
     */
    this.CreateTime = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;
    this.ModelDefine = 'ModelDefine' in params ? params.ModelDefine : null;
    this.UpdateTime = 'UpdateTime' in params ? params.UpdateTime : null;
    this.CreateTime = 'CreateTime' in params ? params.CreateTime : null;

  }
}

/**
 * CreateProject返回参数结构体
 * @class
 */
class CreateProjectResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 返回信息
     * @type {ProjectEntry || null}
     */
    this.Project = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Project) {
      let obj = new ProjectEntry();
      obj.deserialize(params.Project)
      this.Project = obj;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * ReleaseStudioProduct请求参数结构体
 * @class
 */
class ReleaseStudioProductRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

    /**
     * 产品DevStatus
     * @type {string || null}
     */
    this.DevStatus = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;
    this.DevStatus = 'DevStatus' in params ? params.DevStatus : null;

  }
}

/**
 * DescribeModelDefinition请求参数结构体
 * @class
 */
class DescribeModelDefinitionRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;

  }
}

/**
 * DeleteProject返回参数结构体
 * @class
 */
class DeleteProjectResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * GetProjectList返回参数结构体
 * @class
 */
class GetProjectListResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目列表
     * @type {Array.<ProjectEntryEx> || null}
     */
    this.Projects = null;

    /**
     * 列表项个数
     * @type {number || null}
     */
    this.Total = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Projects) {
      this.Projects = new Array();
      for (let z in params.Projects) {
        let obj = new ProjectEntryEx();
        obj.deserialize(params.Projects[z]);
        this.Projects.push(obj);
      }
    }
    this.Total = 'Total' in params ? params.Total : null;
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * DescribeDeviceData返回参数结构体
 * @class
 */
class DescribeDeviceDataResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 设备数据
     * @type {string || null}
     */
    this.Data = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.Data = 'Data' in params ? params.Data : null;
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * SearchStudioProduct返回参数结构体
 * @class
 */
class SearchStudioProductResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品列表
     * @type {Array.<ProductEntry> || null}
     */
    this.Products = null;

    /**
     * 产品数量
     * @type {number || null}
     */
    this.Total = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Products) {
      this.Products = new Array();
      for (let z in params.Products) {
        let obj = new ProductEntry();
        obj.deserialize(params.Products[z]);
        this.Products.push(obj);
      }
    }
    this.Total = 'Total' in params ? params.Total : null;
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * DescribeProject请求参数结构体
 * @class
 */
class DescribeProjectRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目ID
     * @type {number || null}
     */
    this.ProjectId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;

  }
}

/**
 * GetStudioProductList请求参数结构体
 * @class
 */
class GetStudioProductListRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目ID
     * @type {string || null}
     */
    this.ProjectId = null;

    /**
     * 产品DevStatus
     * @type {string || null}
     */
    this.DevStatus = null;

    /**
     * Offset
     * @type {number || null}
     */
    this.Offset = null;

    /**
     * Limit
     * @type {number || null}
     */
    this.Limit = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProjectId = 'ProjectId' in params ? params.ProjectId : null;
    this.DevStatus = 'DevStatus' in params ? params.DevStatus : null;
    this.Offset = 'Offset' in params ? params.Offset : null;
    this.Limit = 'Limit' in params ? params.Limit : null;

  }
}

/**
 * DescribeDeviceData请求参数结构体
 * @class
 */
class DescribeDeviceDataRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品ID
     * @type {string || null}
     */
    this.ProductId = null;

    /**
     * 设备名称
     * @type {string || null}
     */
    this.DeviceName = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProductId = 'ProductId' in params ? params.ProductId : null;
    this.DeviceName = 'DeviceName' in params ? params.DeviceName : null;

  }
}

/**
 * ModifyModelDefinition返回参数结构体
 * @class
 */
class ModifyModelDefinitionResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

/**
 * CreateProject请求参数结构体
 * @class
 */
class CreateProjectRequest extends AbstractModel {
  constructor() {
    super();

    /**
     * 项目名称
     * @type {string || null}
     */
    this.ProjectName = null;

    /**
     * 项目描述
     * @type {string || null}
     */
    this.ProjectDesc = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }
    this.ProjectName = 'ProjectName' in params ? params.ProjectName : null;
    this.ProjectDesc = 'ProjectDesc' in params ? params.ProjectDesc : null;

  }
}

/**
 * DescribeStudioProduct返回参数结构体
 * @class
 */
class DescribeStudioProductResponse extends AbstractModel {
  constructor() {
    super();

    /**
     * 产品详情
     * @type {ProductEntry || null}
     */
    this.Product = null;

    /**
     * 唯一请求 ID，每次请求都会返回。定位问题时需要提供该次请求的 RequestId。
     * @type {string || null}
     */
    this.RequestId = null;

  }

  /**
   * @private
   */
  deserialize(params) {
    if (!params) {
      return;
    }

    if (params.Product) {
      let obj = new ProductEntry();
      obj.deserialize(params.Product)
      this.Product = obj;
    }
    this.RequestId = 'RequestId' in params ? params.RequestId : null;

  }
}

module.exports = {
  DescribeProjectResponse: DescribeProjectResponse,
  ControlDeviceDataResponse: ControlDeviceDataResponse,
  DescribeDeviceDataHistoryRequest: DescribeDeviceDataHistoryRequest,
  DescribeStudioProductRequest: DescribeStudioProductRequest,
  DeleteProjectRequest: DeleteProjectRequest,
  DeviceDataHistoryItem: DeviceDataHistoryItem,
  ProductEntry: ProductEntry,
  GetStudioProductListResponse: GetStudioProductListResponse,
  DescribeModelDefinitionResponse: DescribeModelDefinitionResponse,
  ProjectEntryEx: ProjectEntryEx,
  GetProjectListRequest: GetProjectListRequest,
  DeleteStudioProductResponse: DeleteStudioProductResponse,
  ModifyStudioProductResponse: ModifyStudioProductResponse,
  ControlDeviceDataRequest: ControlDeviceDataRequest,
  SearchStudioProductRequest: SearchStudioProductRequest,
  ReleaseStudioProductResponse: ReleaseStudioProductResponse,
  ProjectEntry: ProjectEntry,
  ModifyProjectResponse: ModifyProjectResponse,
  ModifyModelDefinitionRequest: ModifyModelDefinitionRequest,
  CreateStudioProductResponse: CreateStudioProductResponse,
  DeleteStudioProductRequest: DeleteStudioProductRequest,
  ModifyProjectRequest: ModifyProjectRequest,
  ModifyStudioProductRequest: ModifyStudioProductRequest,
  DescribeDeviceDataHistoryResponse: DescribeDeviceDataHistoryResponse,
  CreateStudioProductRequest: CreateStudioProductRequest,
  ProductModelDefinition: ProductModelDefinition,
  CreateProjectResponse: CreateProjectResponse,
  ReleaseStudioProductRequest: ReleaseStudioProductRequest,
  DescribeModelDefinitionRequest: DescribeModelDefinitionRequest,
  DeleteProjectResponse: DeleteProjectResponse,
  GetProjectListResponse: GetProjectListResponse,
  DescribeDeviceDataResponse: DescribeDeviceDataResponse,
  SearchStudioProductResponse: SearchStudioProductResponse,
  DescribeProjectRequest: DescribeProjectRequest,
  GetStudioProductListRequest: GetStudioProductListRequest,
  DescribeDeviceDataRequest: DescribeDeviceDataRequest,
  ModifyModelDefinitionResponse: ModifyModelDefinitionResponse,
  CreateProjectRequest: CreateProjectRequest,
  DescribeStudioProductResponse: DescribeStudioProductResponse,

}
