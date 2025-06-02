'use client'
import React, {
    createContext,
    useContext,
    useEffect,
    useRef,
    useState,
    ReactNode,
} from 'react'
import { toast } from '@/hooks/use-toast'
import { SelectedDeviceContext } from './SelectedDeviceProvider'

export type DevTelemetry = {
    deviceId: string
    deviceName: string
    pH: number[]
    ORP: number[]
    TUR: number[]
    EC: number[]
}

interface MultiTelemetryContextValue {
    telemetry: DevTelemetry[]
}


const MultiTelemetryContext = createContext<MultiTelemetryContextValue | null>(
    null
)

export function MultiTelemetryProvider({ children }: { children: ReactNode }) {
    const { deviceList, refreshKey } = useContext(SelectedDeviceContext)
    const [telemetry, setTelemetry] = useState<DevTelemetry[]>([])
    const wsRef = useRef<WebSocket>()

    useEffect(() => {
        if (deviceList.length === 0) return
        let mounted = true

        setTelemetry(
            deviceList.map((dev) => ({
                deviceId: dev?.id.id,
                deviceName: dev?.name,
                pH: [], ORP: [], TUR: [], EC: []
            }))
        )

        async function start() {
            try {
                const token = localStorage.getItem('token') ?? ''

                const ws = new WebSocket(
                    `${process.env.NEXT_PUBLIC_TB_WS_URL}` || ''
                )
                // wsRef.current = ws

                // Khi open, gửi một cmds cho mỗi device
                ws.onopen = () => {
                    const cmds =
                        deviceList.flatMap((dev, idx) => [
                            // 1) timeseries
                            {
                                cmdId: idx + 1,
                                // subscriptionId: deviceList[0].id.id,  // dùng để map lại sau
                                entityType: 'DEVICE',
                                entityId: dev.id.id,
                                scope: 'LATEST_TELEMETRY',
                                type: 'TIMESERIES',
                            }
                            // 2) (tuỳ chọn) shared attrs, nếu cần
                            // { ... }
                        ])
                    ws.send(JSON.stringify({ authCmd: { cmdId: 0, token }, cmds }))
                }

                ws.onmessage = (evt) => {
                    if (!mounted) return
                    const msg: any = JSON.parse(evt.data)
                    // Thông điệp telemetry có msg.data + msg.subscriptionId
                    if (msg.data && msg.subscriptionId) {
                        const subId = msg.subscriptionId as number
                        // Tìm device trong mảng bằng cmdId (i+1)
                        const devIndex = subId - 1
                        const dev = deviceList[devIndex]
                        if (!dev) return

                        // Lấy giá trị mới nhất cho mỗi sensor
                        const d = msg.data as Record<string, [number, number][]>
                        const valuePH = d.pH?.[0]?.[1]
                        const valueORP = d.ORP?.[0]?.[1]
                        const valueTUR = d.TUR?.[0]?.[1]
                        const valueEC = d.EC?.[0]?.[1]

                        // Cập nhật state
                        setTelemetry((prev) => {
                            // clone mảng cũ
                            const copy = prev.slice()
                            const entry = { ...copy[devIndex] } // clone object
                            if (valuePH != null) {
                                const last = entry.pH[entry.pH.length - 1]
                                if (last !== valuePH) entry.pH.push(valuePH)
                            }
                            if (valueORP != null) {
                                const last = entry.ORP[entry.ORP.length - 1]
                                if (last !== valueORP) entry.ORP.push(valueORP)
                            }
                            if (valueTUR != null) {
                                const last = entry.TUR[entry.TUR.length - 1]
                                if (last !== valueTUR) entry.TUR.push(valueTUR)
                            }
                            if (valueEC != null) {
                                const last = entry.EC[entry.EC.length - 1]
                                if (last !== valueEC) entry.EC.push(valueEC)
                            }
                            copy[devIndex] = entry
                            return copy
                        })
                    }
                }

                ws.onclose = () => console.log('Multi Telemetry WS closed')
                return () => {
                    ws.close()
                }
            } catch (error) {
                console.log(error)

            }
        }

        start()
        return () => {
            mounted = false
            wsRef.current?.close()
        }
    }, [deviceList, refreshKey])

    return (
        <MultiTelemetryContext.Provider value={{ telemetry }}>
            {children}
        </MultiTelemetryContext.Provider>
    )
}

export function useMultiTelemetry() {
    const ctx = useContext(MultiTelemetryContext)
    if (!ctx) throw new Error('useMultiTelemetry must be under MultiTelemetryProvider')
    return ctx.telemetry
}