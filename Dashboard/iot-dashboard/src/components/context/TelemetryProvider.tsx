'use client'
import { getDeviceInfo } from "@/actions"
import { formatDate } from "@/lib/utils"
import { createContext, useContext, useEffect, useRef, useState, ReactNode } from "react"

type SensorData = {
    time: string,
    value: number
}

interface TelemetryContextValue {
    pH: SensorData[]
    ORP: SensorData[]
    TUR: SensorData[]
    EC: SensorData[]
    predictedPH: SensorData[]
    predictedORP: SensorData[]
    predictedTUR: SensorData[]
    predictedEC: SensorData[]
    currentFWVersion: SensorData | undefined
    fwTitle: SensorData | undefined
    fwVersion: SensorData | undefined
    executed_cronjob: string | undefined
    name: string | undefined
}

const TelemetryContext = createContext<TelemetryContextValue | null>(null)

export function TelemetryProvider({
    deviceId,
    children,
    // dataPH,
    // dataTUR,
    // dataORP,
    // dataEC
}: {
    deviceId: string
    children: ReactNode
    // dataPH: any
    // dataTUR: any
    // dataORP: any
    // dataEC: any
}) {

    // const { toast } = useToast()

    const [pH, setPH] = useState<SensorData[]>([])
    const [ORP, setORP] = useState<SensorData[]>([])
    const [TUR, setTUR] = useState<SensorData[]>([])
    const [EC, setEC] = useState<SensorData[]>([])
    const [predictedPH, setPredictedPH] = useState<SensorData[]>([])
    const [predictedORP, setPredictedORP] = useState<SensorData[]>([])
    const [predictedTUR, setPredictedTUR] = useState<SensorData[]>([])
    const [predictedEC, setPredictedEC] = useState<SensorData[]>([])
    const [currentFWVersion, setCurrentFWVersion] = useState<SensorData | undefined>()
    const [fwVersion, setFWVersion] = useState<SensorData | undefined>()
    const [fwTitle, setFWTitle] = useState<SensorData | undefined>()
    const [executed_cronjob, setExecutedCronjob] = useState<string | undefined>()
    const [name, setName] = useState<string | undefined>()
    const wsRef = useRef<WebSocket>()

    useEffect(() => {
        if (!deviceId) return
        let mounted = true

        const fetchLoginData = async () => {
            try {

                const token = localStorage.getItem('token') ?? ''

                const res = await getDeviceInfo(deviceId, token)
                console.log(' >>>>>>>', res)
                setName(res.name)


                const webSocket = new WebSocket(
                    `${process.env.NEXT_PUBLIC_TB_WS_URL}` || ''
                )

                webSocket.onopen = () => {
                    const object = {
                        authCmd: {
                            cmdId: 0,
                            token: token
                        },
                        cmds: [
                            {
                                entityType: 'DEVICE',
                                entityId: deviceId,
                                scope: 'LATEST_TELEMETRY',
                                cmdId: 1,
                                type: 'TIMESERIES'
                            },
                            {
                                entityType: 'DEVICE',
                                entityId: deviceId,
                                scope: 'SHARED_SCOPE',
                                cmdId: 1,
                                type: 'ATTRIBUTES'
                            },
                        ]
                    }
                    const data = JSON.stringify(object)
                    webSocket.send(data)
                }

                webSocket.onmessage = async (event) => {
                    const receivedData = JSON.parse(event.data)
                    const { subscriptionId, data } = receivedData
                    // console.log('data >>>>', data)
                    if (data?.pH?.length > 0) {
                        setPH((curData) => [...curData, { time: data.pH[0][0], value: data.pH[0][1] }])
                    }
                    if (data?.ORP?.length > 0) {
                        setORP((curData) => [...curData, { time: data.ORP[0][0], value: data.ORP[0][1] }])
                    }
                    if (data?.TUR?.length > 0) {
                        setTUR((curData) => [...curData, { time: data.TUR[0][0], value: data.TUR[0][1] }])
                    }
                    if (data?.EC?.length > 0) {
                        setEC((curData) => [...curData, { time: data.EC[0][0], value: data.EC[0][1] }])
                    }
                    if (data?.predicted_pH?.length > 0) {
                        setPredictedPH((curData) => [...curData, { time: data.predicted_pH[0][0], value: data.predicted_pH[0][1] }])
                    }
                    if (data?.predicted_ORP?.length > 0) {
                        setPredictedORP((curData) => [...curData, { time: data.predicted_ORP[0][0], value: data.predicted_ORP[0][1] }])
                    }
                    if (data?.predicted_TUR?.length > 0) {
                        setPredictedTUR((curData) => [...curData, { time: data.predicted_TUR[0][0], value: data.predicted_TUR[0][1] }])
                    }
                    if (data?.predicted_EC?.length > 0) {
                        setPredictedEC((curData) => [...curData, { time: data.predicted_EC[0][0], value: data.predicted_EC[0][1] }])
                    }
                    if (data?.current_fw_version?.length > 0) {
                        setCurrentFWVersion(data?.current_fw_version[0][1])
                    }
                    if (data?.fw_version?.length > 0) {
                        setFWVersion(data?.fw_version[0][1])
                    }
                    if (data?.fw_title?.length > 0) {
                        setFWTitle(data?.fw_title[0][1])
                    }
                    if (data?.executed_cronjob?.length > 0) {
                        setExecutedCronjob(data?.executed_cronjob[0][1])
                    }
                }

                webSocket.onclose = () => {
                    console.log('Connection closed!')
                }

                return () => {
                    webSocket.close()
                }
            } catch (error) {
                console.log(error)
            }
        }

        fetchLoginData()

        return () => {
            mounted = false
            wsRef.current?.close()
        }
    }, [deviceId])

    return (
        <TelemetryContext.Provider value={{ pH, ORP, TUR, EC, predictedPH, predictedORP, predictedTUR, predictedEC, currentFWVersion, fwVersion, fwTitle, executed_cronjob, name }}>
            {children}
        </TelemetryContext.Provider>
    )
}

export function useTelemetry() {
    const ctx = useContext(TelemetryContext)
    if (!ctx) {
        throw new Error('useTelemetry must be used within a TelemetryProvider')
    }
    return ctx
}